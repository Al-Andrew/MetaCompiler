#include "mc/generate.hpp"

#include "mc/language_description.hpp"
#include "mc/logging.hpp"
#include "mc/stencil.hpp"
#include "mc/utils.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <variant>

namespace mc {

static constexpr std::string_view cmake_project_stencil = R"__cmake(
cmake_minimum_required(VERSION 3.22)

project(/* @project_name */ LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
)__cmake";

static constexpr std::string_view cmake_src_stencil = R"__cmake(
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
# Set the input files
set(PARSER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/parser.y)
set(LEXER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l)

# Set the output files
set(PARSER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
set(LEXER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

# Generate the parser and lexer sources
BISON_TARGET(Parser ${PARSER_INPUT} ${PARSER_OUTPUT} COMPILE_FLAGS "-d")
FLEX_TARGET(Lexer ${LEXER_INPUT} ${LEXER_OUTPUT} COMPILE_FLAGS "--header=lexer.hpp")


set(SOURCES
    ast.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

add_library(language STATIC ${SOURCES})
# Include the generated sources in the build
target_include_directories(language PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})


/* @add_executable */

)__cmake";

void
generate_makefile(Language_Description ld, std::ofstream &proj, std::ofstream &src) noexcept {
    MC_TRACE_FUNCTION("");
    using mc::Stencil;

    Stencil proj_stencil(cmake_project_stencil, proj);

    auto current_ident = proj_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@project_name\" identifier in CMakeLists.txt stencil");
    proj_stencil.file << ld.name;
    current_ident = proj_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in CMakeLists.txt stencil");

    Stencil src_stencil(cmake_src_stencil, src);

    current_ident = src_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@add_executable\" identifier in src/CMakeLists.txt stencil");
    if (ld.main.has_value()) {
        src_stencil.file << "add_executable(" << ld.bin_name << " main.cpp)\n";
        src_stencil.file << "target_link_libraries(" << ld.bin_name << " PRIVATE language)\n";
    }

    current_ident = src_stencil.push_untill_identifier();
}

static constexpr std::string_view lex_stencil = R"__lex(
%{
#include "tokens.hpp"
#include "ast.hpp"
#include "parser.hpp"

unsigned int column_number = 1;
%}

%option noyywrap

%%

/* @tokens */

[ \t] { column_number++; }
\n {yylineno++; column_number = 1;}
"//".*$ {;}
. { column_number++; return yytext[0];}

%%
)__lex";

static constexpr std::string_view yacc_stencil = R"__yacc(
%{
#include <stdio.h>
#include "ast.hpp"
#include "tokens.hpp"
#include "lexer.hpp"
#include <fstream>

extern int yyerror(const char * s);
extern unsigned int column_number;
Ast_Node* ast_root = nullptr;
std::ofstream out_stream;
%}

%union {
    Ast_Node* ast_node;
}

/* @tokens_types */
/* @rules_types */

%start /* @start_symbol */
%%

/* @rules */

%%
int yyerror(const char * s) {

    printf("[Line: %d][Col: %u] Error: %s. Arround %s\n", yylineno, column_number, s, yytext);
    yylineno = 1;
    column_number = 1;
    return 1;
}
)__yacc";

void
generate_lex_lexer_yacc_parser(Language_Description ld, std::ofstream &lexer, std::ofstream &parser) noexcept {
    MC_TRACE_FUNCTION("");

    using mc::Stencil;
    Stencil lexer_stencil(lex_stencil, lexer);
    Stencil parser_stencil(yacc_stencil, parser);

    auto current_ident = lexer_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@tokens\" identifier in lexer.l stencil");

    for (auto token : ld.tokens) {
        lexer_stencil.file << token.matcher_text() << " { column_number += yyleng; "
                           << "yylval.ast_node = Ast_Node_Token::make(Token{" << token.full_enum_name()
                           << ", yytext, (unsigned int)yylineno, column_number}); "
                           << "return " << token.enum_name() << "; }\n";
    }
    current_ident = lexer_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in lexer.l stencil");

    // ---------------------------------------------------------------------------------------------

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@tokens_types\" identifier in parser.y stencil");
    for (const auto &token : ld.tokens) {
        parser_stencil.file << "%token <ast_node>" << token.enum_name() << "\n";
    }

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@rules_types\" identifier in parser.y stencil");
    for (const auto &rule : ld.rules) {
        parser_stencil.file << "%type <ast_node>" << rule.name << "\n";
    }

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@start_symbol\" identifier in parser.y stencil");
    parser_stencil.file << ld.start_rule;

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@rules\" identifier in parser.y stencil");
    for (const auto &rule : ld.rules) {
        parser_stencil.file << rule.name << " : ";
        for (unsigned int i = 0; i < rule.constructions.size(); i++) {
            const auto &construction = rule.constructions[i];
            for (const auto &symbol : construction.symbols_variant) {
                if (const Token *t = std::get_if<Token>(&symbol)) {
                    parser_stencil.file << t->enum_name() << " ";
                } else if (const Rule *r = std::get_if<Rule>(&symbol)) {
                    parser_stencil.file << r->name << " ";
                }
            }
            parser_stencil.file << "{ $$ = " << construction.ast_node_name(rule.name) << "::make(";
            for (unsigned int i = 0; i < construction.symbols_variant.size(); i++) {
                parser_stencil.file << "$" << i + 1;
                if (i != construction.symbols_variant.size() - 1) parser_stencil.file << ", ";
            }
            parser_stencil.file << ");";

            if (rule.name == ld.start_rule) {
                parser_stencil.file << " ast_root = $$; ";
            }

            parser_stencil.file << "}\n";

            if (i != rule.constructions.size() - 1) {
                parser_stencil.file << "| ";
            }
        }
    }

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in parser.y stencil");
}

static constexpr std::string_view tokens_header_stencil = R"__hpp(
#pragma once
#include <string>

enum class Token_Type : unsigned int {
/* @tokens */
};

struct Token {
    Token_Type type;
    std::string value;
    unsigned int line;
    unsigned int column;
};

)__hpp";

static constexpr std::string_view tokens_file_stencil = R"__cpp(

)__cpp";

void
generate_tokens(Language_Description ld, std::ofstream &header, std::ofstream &file) noexcept {
    MC_TRACE_FUNCTION("");
    using mc::Stencil;

    Stencil header_stencil(tokens_header_stencil, header);
    Stencil file_stencil(tokens_file_stencil, file);

    auto current_ident = header_stencil.push_untill_identifier();

    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@tokens\" identifier in tokens.hpp stencil");
    for (auto token : ld.tokens) {
        header_stencil.file << "    " << token.enum_name() << ",\n";
    }
    current_ident = header_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in tokens.hpp stencil");

    current_ident = file_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in tokens.cpp stencil");
}

static constexpr std::string_view ast_header_stencil = R"__hpp(
#pragma once
#include "tokens.hpp"

#include <vector>
#include <iostream>

struct Ast_Node {
    std::vector<Ast_Node*> children;

    Ast_Node(std::vector<Ast_Node*> children) : children(children) {}
    virtual ~Ast_Node();

    virtual const char* get_name() = 0;

    virtual void print(unsigned int indent = 0) {
        std::string indent_str(indent, ' ');
        std::cout << indent_str << "-" << get_name() << "\n";
        for (auto child : children) {
            child->print(indent + 1);
        }
    }

    virtual void traverse() = 0;
};

struct Ast_Node_Token : public Ast_Node {
    Token token;

    Ast_Node_Token(std::vector<Ast_Node*> children, Token token) : Ast_Node(children), token(token) {}

    static Ast_Node_Token* make(Token token) {
        return new Ast_Node_Token{std::vector<Ast_Node*>{}, token};
    }

    virtual const char* get_name() override {
        return "Ast_Node_Token";
    }

    virtual void print(unsigned int indent = 0) override {
        std::string indent_str(indent, ' ');
        std::cout << indent_str << "-" << get_name() << " : " << token.value << "\n";
    }

    virtual void traverse() override {}
};

/* @ast_nodes */
)__hpp";

static constexpr std::string_view ast_file_stencil = R"__cpp(
#include "ast.hpp"
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

std::unordered_map<std::string, std::string> user_data;
std::vector<std::string> includes;
extern std::ofstream out_stream;
extern void push(int i);
extern int pop();
extern int stack[1024];
extern int stack_pointer;
extern Ast_Node* ast_root;
extern FILE* yyin;
extern int yyparse();

static std::string escape_cpp_literal(const std::string& literal) {
    std::string ret;

    for(const char c : literal) {
        switch(c) {
            case '\n': ret += "\\n"; break;
            case '\t': ret += "\\t"; break;
            case '\r': ret += "\\r"; break;
            case '\v': ret += "\\v"; break;
            case '\b': ret += "\\b"; break;
            case '\f': ret += "\\f"; break;
            case '\a': ret += "\\a"; break;
            case '\\': ret += "\\\\"; break;
            case '\'': ret += "\\'"; break;
            case '\"': ret += "\\\""; break;
            default: ret += c; break;
        }
    }

    return ret;
}

#include <algorithm>

std::vector<std::string> split_params(const std::string& str) {
    std::vector<std::string> constructs;
    std::string buffer;

    bool inBackticks = false;

    for (char ch : str) {
        if (ch == '`') {
            inBackticks = !inBackticks;
        }
        if (ch == ',' && !inBackticks) {
            // Remove leading/trailing spaces
            buffer.erase(buffer.begin(),
                         std::find_if(buffer.begin(), buffer.end(), [](int ch) {
                             return !std::isspace(ch);
                         }));
            buffer.erase(std::find_if(buffer.rbegin(), buffer.rend(),
                                      [](int ch) { return !std::isspace(ch); })
                             .base(),
                         buffer.end());
            buffer = buffer.substr(1, buffer.size() - 2);
            constructs.push_back(buffer);
            buffer.clear();
        } else {
            buffer += ch;
        }
    }
    // Push the last construct
    // Remove leading/trailing spaces
    buffer.erase(buffer.begin(),
                 std::find_if(buffer.begin(), buffer.end(),
                              [](int ch) { return !std::isspace(ch); }));
    buffer.erase(std::find_if(buffer.rbegin(), buffer.rend(),
                              [](int ch) { return !std::isspace(ch); })
                     .base(),
                 buffer.end());
    if(buffer[0] == '`') {
        buffer = buffer.substr(1, buffer.size() - 2);
    }
    printf("is_last_construct: %s\n", buffer.c_str());
    constructs.push_back(buffer);

    return constructs;
}

static std::string expand_component_action(const std::string& code, const std::string& paramList) {
    // the paramList is of the form: p1,p2,p3,p4
    // first we need to split it into a vector of strings
    printf("before split paramList: %s\n", paramList.c_str());
    std::vector<std::string> params = split_params(paramList);
    

    // now we need to replace the $0, $1, $2 etc. with the params
    std::string ret = "";
    
    for(int idx = 0; idx < code.size() ; ++idx) {
        if(code[idx] == '$') {
            if(code[idx + 1] == '$') {
                ret += '$';
                ++idx;
            } else {
                std::string param_idx_str = "";
                while(idx + 1 < code.size() && code[idx + 1] >= '0' && code[idx + 1] <= '9') {
                    param_idx_str += code[idx + 1];
                    ++idx;
                }
                int param_idx = std::stoi(param_idx_str);
                ret += params[param_idx];
                printf("replaced param %d with %s\n", param_idx, params[param_idx].c_str());
            }
        } else {
            ret += code[idx];
        }
    }

    return ret;
}

Ast_Node::~Ast_Node(){
    for (auto child : children) {
        delete child;
    }
}

)__cpp";

void
generate_ast(Language_Description ld, std::ofstream &header, std::ofstream &file) noexcept {
    MC_TRACE_FUNCTION("");

    using mc::Stencil;
    Stencil header_stencil(ast_header_stencil, header);
    Stencil file_stencil(ast_file_stencil, file);

    auto current_ident = header_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@ast_nodes\" identifier in ast.hpp stencil");
    current_ident = file_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in ast.cpp stencil");
    for (const auto &rule : ld.rules) {
        header_stencil.file << "struct " << rule.ast_node_name() << " : public Ast_Node {\n";
        {  // constructor
            header_stencil.file << "    " << rule.ast_node_name() << "(std::vector<Ast_Node*> children) : "
                                << "Ast_Node(children) {}\n";
        }
        {  // virtual destructor
            header_stencil.file << "    virtual ~" << rule.ast_node_name() << "();\n";
            file_stencil.file << rule.ast_node_name() << "::~" << rule.ast_node_name() << "() {}\n";
        }
        header_stencil.file << "};\n";

        for (const auto &construction : rule.constructions) {
            // start struct
            header_stencil.file << "struct " << construction.ast_node_name(rule.name) << " : public "
                                << rule.ast_node_name() << " {\n";
            {  // constructor
                header_stencil.file << "    " << construction.ast_node_name(rule.name)
                                    << "(std::vector<Ast_Node*> children) : " << rule.ast_node_name()
                                    << "(children) {}\n\n";
            }
            {  // virtual destructor
                header_stencil.file << "    virtual ~" << construction.ast_node_name(rule.name) << "();\n\n";
                file_stencil.file << construction.ast_node_name(rule.name) << "::~"
                                  << construction.ast_node_name(rule.name) << "() {}\n\n";
            }
            {  // make(...)
                header_stencil.file << "    static " << construction.ast_node_name(rule.name) << "* make(";
                for (unsigned int i = 0; i < construction.symbols_variant.size(); ++i) {
                    const auto &symbol = construction.symbols_variant[i];

                    header_stencil.file << "Ast_Node"
                                        << "* "
                                        << "p" << i;

                    if (i != construction.symbols_variant.size() - 1) {
                        header_stencil.file << ", ";
                    }
                }
                header_stencil.file << ");\n\n";

                file_stencil.file << "    " << construction.ast_node_name(rule.name) << "* "
                                  << construction.ast_node_name(rule.name) << "::make(";

                for (unsigned int i = 0; i < construction.symbols_variant.size(); ++i) {
                    const auto &symbol = construction.symbols_variant[i];

                    file_stencil.file << "Ast_Node"
                                      << "* "
                                      << "p" << i;

                    if (i != construction.symbols_variant.size() - 1) {
                        file_stencil.file << ", ";
                    }
                }

                file_stencil.file << ") {\n";
                file_stencil.file << "        return new " << construction.ast_node_name(rule.name)
                                  << "{std::vector<Ast_Node*>{";

                for (unsigned int i = 0; i < construction.symbols_variant.size(); ++i) {
                    const auto &symbol = construction.symbols_variant[i];
                    file_stencil.file << "p" << i;
                    ;

                    if (i != construction.symbols_variant.size() - 1) {
                        file_stencil.file << ", ";
                    }
                }
                file_stencil.file << "}};\n    }\n";
            }
            {  // get_name
                header_stencil.file << "    virtual const char* get_name() override;\n";

                file_stencil.file << "    const char* " << construction.ast_node_name(rule.name) << "::get_name() {\n";
                file_stencil.file << "        return \"" << construction.ast_node_name(rule.name) << "\";\n";
                file_stencil.file << "    }\n";
            }
            {  // traverse
                header_stencil.file << "    virtual void traverse() override;\n";

                file_stencil.file << "    void " << construction.ast_node_name(rule.name) << "::traverse() {\n";
                MC_DEBUG("format_action: {}", construction.format_action());
                MC_DEBUG("action: {}", construction.action);
                file_stencil.file << construction.format_action();
                file_stencil.file << "\n    }\n";
            }
            // end struct
            header_stencil.file << "};\n\n";
        }
        header_stencil.file << "\n";
    }
}

void
generate_main(Language_Description ld, std::filesystem::path output_dir) {
    if (ld.main.has_value()) {
        std::ofstream file(output_dir / "src" / "main.cpp");
        file << ld.main.value();
    }
}

void
generate_executable(std::filesystem::path output_dir) noexcept {
    MC_TRACE_FUNCTION("");
    std::filesystem::path original_path = std::filesystem::current_path();
    std::filesystem::current_path(output_dir);

    std::filesystem::create_directory("build");
    std::system("cmake -DCMAKE_BUILD_TYPE=Debug -B build/ -S .");
    std::system("cmake --build build/");

    std::filesystem::current_path(original_path);
}

void
generate(Language_Description ld, std::filesystem::path output_dir) noexcept {
    MC_TRACE_FUNCTION("");

    MC_CHECK_WARN(
        std::filesystem::create_directory(output_dir),
        "Failed to create output directory \"{}\". If it already exists it will be overriden.", output_dir.string());
    MC_CHECK_WARN(
        std::filesystem::create_directory(output_dir / "src"),
        "Failed to create output directory \"{}/src\". If it already exists it will be overriden.",
        output_dir.string());

    {  // NOTE: generate we need the scope to close the files
        std::ofstream proj_makefile(output_dir / "CMakeLists.txt");
        std::ofstream src_makefile(output_dir / "src" / "CMakeLists.txt");
        generate_makefile(ld, proj_makefile, src_makefile);
    }
    {
        std::ofstream tokens_header(output_dir / "src" / "tokens.hpp");
        std::ofstream tokens_file(output_dir / "src" / "tokens.cpp");
        generate_tokens(ld, tokens_header, tokens_file);
    }
    {
        std::ofstream ast_header(output_dir / "src" / "ast.hpp");
        std::ofstream ast_file(output_dir / "src" / "ast.cpp");
        generate_ast(ld, ast_header, ast_file);
    }
    {
        std::ofstream lexer(output_dir / "src" / "lexer.l");
        std::ofstream parser(output_dir / "src" / "parser.y");
        generate_lex_lexer_yacc_parser(ld, lexer, parser);
    }
    { generate_main(ld, output_dir); }

    generate_executable(output_dir);
}

}  // namespace mc