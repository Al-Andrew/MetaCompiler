#include "mc/generate.hpp"

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

# Include the generated sources in the build
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    ast.cpp
)

add_executable(/* @bin */ ${SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
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
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@bin_name\" identifier in src/CMakeLists.txt stencil");
    src_stencil.file << ld.bin_name;
    current_ident = src_stencil.push_untill_identifier();
}

static constexpr std::string_view lex_stencil = R"__lex(
%{
#include "parser.hpp"
#include "ast.hpp"
%}

%option noyywrap

%%

/* @tokens */

[ \t] ;
\n {yylineno++;}
"//".*$ {;}
. {return yytext[0];}

%%
)__lex";

static constexpr std::string_view yacc_stencil = R"__yacc(
%{
#include <stdio.h>
#include "ast.hpp"
#include "lexer.hpp"

extern int yyerror(char * s);
%}

%union {
    struct _Ast_Node* node;
} 

/* @tokens_types */
/* @rules_types */

%start /* @start_symbol */
%%

/* @rules */

%%
int yyerror(char * s) {
    printf("[Line: %d] Error: %s\n", yylineno, s);
}

int main(int argc, char** argv){
    
    if(argc < 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }
    
    yyin=fopen(argv[1],"r");
    yyparse();
    printf("Parsing complete.\n\n");
    // ast_node_print(ast_root, 0);

    FILE* stream = fopen(argv[2], "w");
    // translate_AST_NODE(stream, ast_root);
    return 0;
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
        lexer_stencil.file << token.matcher_text() << " { return " << token.enum_name() << "; }\n";
    }
    current_ident = lexer_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in lexer.l stencil");

    // ---------------------------------------------------------------------------------------------

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@tokens_types\" identifier in parser.y stencil");
    for (const auto &token : ld.tokens) {
        parser_stencil.file << "%token <node>" << token.enum_name() << "\n";
    }

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@rules_types\" identifier in parser.y stencil");
    for (const auto &rule : ld.rules) {
        parser_stencil.file << "%type <node>" << rule.name << "\n";
    }

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@start_symbol\" identifier in parser.y stencil");
    parser_stencil.file << ld.start_rule;

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@rules\" identifier in parser.y stencil");
    for (const auto &rule : ld.rules) {
        parser_stencil.file << rule.name << " : ";
        for (const auto &construction : rule.constructions) {
            for (const auto &symbol : construction.symbols_variant) {
                if (const Token *t = std::get_if<Token>(&symbol)) {
                    parser_stencil.file << t->enum_name() << " ";
                } else if (const Rule *r = std::get_if<Rule>(&symbol)) {
                    parser_stencil.file << r->name << " ";
                }
            }
            // TODO: add actions
            parser_stencil.file << "{;}\n";
        }
    }

    current_ident = parser_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in parser.y stencil");
}

static constexpr std::string_view tokens_header_stencil = R"__hpp(
#pragma once
#include <string_view>

enum class Token_Type : unsigned int {
/* @tokens */
};

struct Token {
    Token_Type type;
    std::string_view value;
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

static constexpr std::string_view symbols_header_stencil = R"__hpp(
#pragma once
#include "tokens.hpp"

enum class Symbol_Type : unsigned int {
    SYMBOL_TOKEN,
    SYMBOL_RULE,
};

struct Symbol {
    Symbol_Type type;
    union {
        Token m_token;
        // Rule m_rule;
    };

    [[nodiscard]] inline bool is_token() const noexcept { return this.type == SYMBOL_TOKEN; };
    [[nodiscard]] inline Token& token() noexcept { return this.m_token; }
    [[nodiscard]] inline const Token& token() const noexcept { return this.m_token; }
    [[nodiscard]] inline bool is_rule() const noexcept { return this.type == SYMBOL_RULE; };
    // [[nodiscard]] inline Rule& rule() noexcept { return this.rule; }
    // [[nodiscard]] inline const Rule& rule() const noexcept { return this.rule; }
};

)__hpp";
static constexpr std::string_view symbols_file_stencil   = R"__cpp()__cpp";

void
generate_symbols(std::ofstream &headern, std::ofstream &file) noexcept {
    MC_TRACE_FUNCTION("");
    using mc::Stencil;

    Stencil header_stencil(symbols_header_stencil, headern);
    Stencil file_stencil(symbols_file_stencil, file);

    auto current_ident = header_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in symbols.hpp stencil");
    current_ident = file_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in symbols.cpp stencil");
}

static constexpr std::string_view ast_header_stencil = R"__hpp(
#pragma once
)__hpp";

static constexpr std::string_view ast_file_stencil = R"__cpp(
#include "ast.hpp"
)__cpp";

void
generate_ast(Language_Description ld, std::ofstream &header, std::ofstream &file) noexcept {
}

void
generate_executable(std::filesystem::path output_dir) noexcept {
    MC_TRACE_FUNCTION("");
    std::filesystem::path original_path = std::filesystem::current_path();
    MC_DEBUG("cwd \"{}\"", std::filesystem::current_path().string());
    std::filesystem::current_path(output_dir);
    MC_DEBUG("cwd \"{}\"", std::filesystem::current_path().string());

    std::filesystem::create_directory("build");
    std::system("cmake -B build/ -S .");
    MC_DEBUG("cwd \"{}\"", std::filesystem::current_path().string());
    std::system("cmake --build build/");

    std::filesystem::current_path(original_path);
    MC_DEBUG("cwd \"{}\"", std::filesystem::current_path().string());
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
        std::ofstream tokens_header(output_dir / "src" / "symbols.hpp");
        std::ofstream tokens_file(output_dir / "src" / "symbols.cpp");
        generate_symbols(tokens_header, tokens_file);
    }
    {
        std::ofstream lexer(output_dir / "src" / "lexer.l");
        std::ofstream parser(output_dir / "src" / "parser.y");
        generate_lex_lexer_yacc_parser(ld, lexer, parser);
    }

    generate_executable(output_dir);
}

}  // namespace mc