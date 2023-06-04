// this project:
#include "mc/generate.hpp"
#include "mc/language_description.hpp"
#include "mc/logging.hpp"
#include "mc/options.hpp"
#include "mc/utils.hpp"
// thirdparty:
#include "nlohmann/json.hpp"
using json = nlohmann::json;
// std:
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#define LOG_INFO std::clog << "[INFO] [" << __FILE__ << ':' << __LINE__ << "] "

std::string
to_upper(std::string const &str) {
    std::string result = str;
    for (char &c : result) {
        c = std::toupper(c);
    }
    return result;
}

std::string
rule_name_and_tag_to_tagid(std::string const &rule_name, std::string const &tag) {
    return std::string{"TAG_ID_"} + to_upper(rule_name) + "_" + to_upper(tag);
}

struct TokenList {
    std::set<std::string>             valid_tokens;
    static std::shared_ptr<TokenList> instance;

    static std::shared_ptr<TokenList>
    the() {
        return instance;
    }

    void
    add_token(std::string const &token) {
        valid_tokens.insert(token);
    }

    bool
    is_valid_token(std::string const &token) {
        return valid_tokens.find(token) != valid_tokens.end();
    }

    static std::string
    symbol_name_to_token_id(std::string const &symbol_name) {
        return std::string{"TKN_ID_"} + symbol_name;
    }
};

std::shared_ptr<TokenList> TokenList::instance = std::make_shared<TokenList>(TokenList{});

static std::ofstream lexer_stream;
static std::ofstream parser_stream;
static std::ofstream ast_c_stream;
static std::ofstream ast_h_stream;
static std::ofstream rules_c_stream;
static std::ofstream rules_h_stream;
static std::ofstream makefile_stream;

void
init_directory_structure(std::string output_directory) {
    std::filesystem::create_directory(output_directory);
    std::filesystem::create_directory(output_directory + "/src");
    lexer_stream.open(output_directory + "/src/lexer.l");
    parser_stream.open(output_directory + "/src/parser.y");
    ast_c_stream.open(output_directory + "/src/Ast.cpp");
    ast_h_stream.open(output_directory + "/src/Ast.hpp");
    rules_c_stream.open(output_directory + "/src/Rules.cpp");
    rules_h_stream.open(output_directory + "/src/Rules.hpp");
    // makefile_stream.open(output_directory + "/xmake.lua");

    LOG_INFO << "Created directory structure.\n";
}

void
begin_lexer_parser() {
    lexer_stream << R"(
%{
#include "parser.hpp"
#include "Ast.hpp"
%}
%option noyywrap
%%

)";

    parser_stream << R"(
%{
#include <stdio.h>
#include "Ast.hpp"
#include "Rules.hpp"
#include "lexer.hpp"

extern int yyerror(char * s);
%}

%union {
    struct _Ast_Node* node;
}

)";

    LOG_INFO << "Beginning lexer + parser.\n";
}

void
generate_lexer(json const &language_description) {
    std::vector<json> const tokens = language_description["tokens"];

    for (json const &token : tokens) {
        std::string const symbol_name   = token["name"];
        std::string const token_matcher = token["matcher"];
        bool const        is_regex      = token["is_regex"].get<bool>();

        std::string const token_id = TokenList::symbol_name_to_token_id(symbol_name);
        TokenList::the()->add_token(symbol_name);
        parser_stream << "%token <node>" << token_id << '\n';
        lexer_stream << (is_regex ? "" : "\"") << token_matcher << (is_regex ? "" : "\"")
                     << " { printf(\" lexing: " << token_id << "\\n\");"
                     << " yylval.node = ast_node_new_token(\"" << token_id << "\", yytext);"
                     << " return " << token_id << "; }\n";

        LOG_INFO << "writing token: " << token_id << " | matcher: " << token_matcher << '\n';
    }

    std::vector<json> const rules = language_description["rules"];
    for (json const &rule : rules) {
        std::string const rule_name = rule["name"];

        parser_stream << "%type <node>" << rule_name << '\n';
    }

    LOG_INFO << "Tokens writen to lexer + parser.\n";
}

void
generate_parser_options(json const &language_description) {
    parser_stream << R"(

%start )" << language_description["start_rule"].get<std::string>()
                  << R"(

%%

)";
}

void
generate_parser(json const &language_description) {
    std::vector<json> const rules = language_description["rules"];

    for (json const &rule : rules) {
        std::string             rule_name     = rule["name"];
        std::vector<json> const constructions = rule["constructions"];

        parser_stream << rule_name << " : ";
        LOG_INFO << "writing rules for grammar piece: " << rule_name << '\n';

        auto write_construction = [&](json const &construction) {
            std::string const              tag     = construction["tag"];
            std::string const              tag_id  = rule_name_and_tag_to_tagid(rule_name, tag);
            std::vector<std::string> const symbols = construction["symbols"];

            for (auto const &symbol : symbols) {
                std::string possible_token_id = TokenList::symbol_name_to_token_id(symbol);
                if (TokenList::the()->is_valid_token(symbol)) {
                    parser_stream << possible_token_id << ' ';
                } else {
                    parser_stream << symbol << ' ';
                }
            }
            parser_stream << "{ printf(\"parsing: " << tag_id << "\\n\"); "
                          << "$$ = ast_node_new(\"" << tag_id << "\", "
                          << "AST_NODE_" << to_upper(rule_name) << ", " << tag_id << ", " << symbols.size() << ", ";
            for (size_t idx = 0; idx < symbols.size(); ++idx) {
                parser_stream << "$" << idx + 1;
                if (idx != symbols.size() - 1) {
                    parser_stream << ", ";
                }
            }

            parser_stream << ");";

            if (rule_name == language_description["start_rule"].get<std::string>()) {
                parser_stream << "ast_root = $$;";
            }

            parser_stream << "}\n";
        };

        write_construction(constructions[0]);
        for (size_t idx = 1; idx < constructions.size(); ++idx) {
            json const &construction = constructions[idx];
            parser_stream << "| ";
            write_construction(construction);
        }
        parser_stream << ";\n\n";
    }
}

void
finalize_lexer_parser() {
    lexer_stream << R"(
[ \t] ;
\n {yylineno++;}
"//".*$ {;}
. {return yytext[0];}

%%
)";
    parser_stream << R"(
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
    ast_node_print(ast_root, 0);

    FILE* stream = fopen(argv[2], "w");
    translate_AST_NODE(stream, ast_root);
    return 0;
}
)";

    LOG_INFO << "Finalizing lexer + parser.\n";
}

void
generate_ast_h(json const &language_description) {
    ast_h_stream << "#ifndef _AST_H_\n";
    ast_h_stream << "#define _AST_H_\n\n";

    ast_h_stream << R"(
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
)";

    std::vector<json> const rules = language_description["rules"];

    // Note(AAL): generate enum for all tags
    ast_h_stream << "typedef enum _Ast_Node_Type {\n    AST_NODE_SIMPLE_TOKEN,\n";

    for (json const &rule : rules) {
        std::string       rule_name = rule["name"];
        std::string const rule_id   = "AST_NODE_" + to_upper(rule_name);
        ast_h_stream << "    " << rule_id << ",\n";
    }
    ast_h_stream << "} Ast_Node_Type;\n\n";
    // ================================

    // Note(AAL): generate one enum for each rule containg its tags
    for (json const &rule : rules) {
        std::string             rule_name     = rule["name"];
        std::string const       rule_id       = "AST_NODE_" + to_upper(rule_name);
        std::vector<json> const constructions = rule["constructions"];

        ast_h_stream << "typedef enum _" << rule_id << "_Tag {\n";
        for (json const &construction : constructions) {
            std::string const tag    = construction["tag"];
            std::string const tag_id = rule_name_and_tag_to_tagid(rule_name, tag);
            ast_h_stream << "    " << tag_id << ",\n";
        }
        ast_h_stream << "} " << rule_id << "_Tag;\n\n";
    }
    // ================================

    // Note: generate the struct and function declarations
    ast_h_stream << R"(
typedef struct _Ast_Node {
    Ast_Node_Type type;
    uint32_t tag;
    const char* name;
    char* value;

    struct _Ast_Node* parent;
    struct _Ast_Node** children;
    uint32_t num_children;
} Ast_Node;

extern Ast_Node* ast_root;
)";
    ast_h_stream << R"(
Ast_Node* ast_node_new(const char* name, uint32_t type, uint32_t tag, uint32_t num_children, ...);
Ast_Node* ast_node_new_token(const char* name, const char* value);
void ast_node_add_child(Ast_Node* parent, Ast_Node* child);
void ast_node_print(Ast_Node* node, uint32_t depth);

const char* token_id_to_string(uint32_t token_id);
const char* ast_node_type_to_string(Ast_Node_Type type);

)";

    ast_h_stream << "#endif //! _AST_H_\n";
}

void
generate_ast_c(json const &language_description) {
    ast_c_stream << R"(
#include "Ast.hpp"
#include "parser.hpp"

#include "string.h"
#include "stdio.h"  

Ast_Node* ast_root = NULL;

Ast_Node* ast_node_new(const char* name, uint32_t type, uint32_t tag, uint32_t num_children, ...) {
    Ast_Node* node = (Ast_Node*)malloc(sizeof(Ast_Node));
    node->name = name;
    node->type = (Ast_Node_Type)type;
    node->tag = tag;
    node->value = NULL;
    node->parent = NULL;
    node->children = (Ast_Node**)malloc(sizeof(Ast_Node*) * num_children);
    node->num_children = 0;

    va_list args;
    va_start(args, num_children);
    for (uint32_t idx = 0; idx < num_children; ++idx) {
        Ast_Node* child = va_arg(args, Ast_Node*);
        ast_node_add_child(node, child);
    }
    va_end(args);

    return node;
}

Ast_Node* ast_node_new_token(const char* name, const char* value) {
    Ast_Node* node = (Ast_Node*)malloc(sizeof(Ast_Node));
    node->name = name;
    node->type = AST_NODE_SIMPLE_TOKEN;
    node->value = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(node->value, value);
    node->tag = 0;
    node->parent = NULL;
    node->children = NULL;
    
    return node;
}

void ast_node_add_child(Ast_Node* parent, Ast_Node* child) {
    parent->children[parent->num_children++] = child;
    child->parent = parent;
}

const char* token_id_to_string(uint32_t token_id) {
    switch (token_id) {
)";

    std::vector<json> const tokens = language_description["tokens"];
    for (json const &token : tokens) {
        std::string const token_name = token["name"];
        std::string const token_id   = TokenList::symbol_name_to_token_id(token_name);
        ast_c_stream << "        case " << token_id << ": return \"" << token_id << "\";\n";
    }
    ast_c_stream << R"(
        default: return NULL;
    };
}
    
const char* ast_node_type_to_string(Ast_Node_Type type) {
    switch(type) {
)";
    std::vector<json> const rules = language_description["rules"];
    for (json const &rule : rules) {
        std::string       rule_name = rule["name"];
        std::string const rule_id   = "AST_NODE_" + to_upper(rule_name);
        ast_c_stream << "        case " << rule_id << ": return \"" << rule_id << "\";\n";
    }
    ast_c_stream << R"(
        default: return NULL;
    }
}

void ast_node_print(Ast_Node* node, uint32_t depth) {
    if (node->type == AST_NODE_SIMPLE_TOKEN)
        return;
    
    for(uint32_t idx = 0; idx < depth; ++idx)
        printf("|");
    printf("- %s\n", node->name);

    for(uint32_t idx = 0; idx < node->num_children; ++idx)
        ast_node_print(node->children[idx], depth + 1);

}

)";
}

void
generate_rules_h(json const &language_description) {
    std::vector<json> const rules = language_description["rules"];

    rules_h_stream << R"(
#ifndef _RULES_H_
#define _RULES_H_

#include "Ast.hpp"
#include "stdio.h"

typedef void (*rule_apply_fn)(FILE* stream, Ast_Node* node);

void translate_AST_NODE_SIMPLE_TOKEN(FILE* stream, Ast_Node* node);

void translate_AST_NODE(FILE* stream, Ast_Node* node);
extern rule_apply_fn rule_apply_fn_AST_NODE[)"
                   << rules.size() + 1 << "];\n\n";

    for (json const &rule : rules) {
        std::string       rule_name = rule["name"];
        std::string const rule_id   = "AST_NODE_" + to_upper(rule_name);

        rules_h_stream << "void translate_" << rule_id << "(FILE* stream, Ast_Node* node);\n";
        for (json const &construction : rule["constructions"]) {
            std::string       construction_name = construction["tag"];
            std::string const construction_id   = "tagged_" + to_upper(construction_name);

            rules_h_stream << "void translate_" << rule_id << "_" << construction_id
                           << "(FILE* stream, Ast_Node* node);\n";
        }

        rules_h_stream << "extern rule_apply_fn rule_apply_fn_" << rule_id << "[" << rule["constructions"].size()
                       << "];\n\n";
    }
    rules_h_stream << R"(


#endif //! _RULES_H_
)";
}

void
generate_rules_c(json const &language_description) {
    std::vector<json> const rules = language_description["rules"];

    rules_c_stream << R"(
#include "Rules.hpp"

void translate_AST_NODE_SIMPLE_TOKEN(FILE* stream, Ast_Node* node) {
    fprintf(stream, "%s", node->value);
}

void translate_AST_NODE(FILE* stream, Ast_Node* node) {
    rule_apply_fn fn = rule_apply_fn_AST_NODE[node->type];
    fn(stream, node);
}

)";

    // generate the function definitions
    for (auto const &rule : rules) {
        std::string const rule_name = rule["name"];
        std::string const rule_id   = "AST_NODE_" + to_upper(rule_name);

        // the function for this rule
        rules_c_stream << "void translate_" << rule_id << "(FILE* stream, Ast_Node* node) {\n";
        rules_c_stream << "    rule_apply_fn fn = rule_apply_fn_" << rule_id << "[node->tag];\n";
        rules_c_stream << "    fn(stream, node);\n";
        rules_c_stream << "}\n\n";

        // the functions for this rule's tags
        for (json const &construction : rule["constructions"]) {
            std::string       construction_name = construction["tag"];
            std::string const construction_id   = "tagged_" + to_upper(construction_name);

            rules_c_stream << "void translate_" << rule_id << "_" << construction_id
                           << "(FILE* stream, Ast_Node* node) {\n";
            // TODO(AAL): parse out the translation out of the construction and
            // generate the code here

            std::string const action = construction["action"];
            std::stringstream action_istream{action};
            std::stringstream action_ostream{};

            std::string slice;
            char        c;
            while (action_istream.get(c), action_istream.good()) {
                if (c == '$') {
                    slice = action_ostream.str();
                    action_ostream.clear();
                    action_ostream.str(std::string());
                    if (slice.size() > 0) rules_c_stream << "    fprintf(stream, \"" << slice << "\");\n";
                    int idx;
                    action_istream >> idx;
                    rules_c_stream << "translate_AST_NODE(stream, node->children[" << idx << "]);\n";
                } else {
                    action_ostream << c;
                }
            }
            slice = action_ostream.str();
            if (slice.size() > 0) rules_c_stream << "    fprintf(stream, \"" << slice << "\");\n";

            rules_c_stream << "}\n\n";
        }

        // the table for this rule's tags
        rules_c_stream << "rule_apply_fn rule_apply_fn_" << rule_id << "[" << rule["constructions"].size() << "] = {\n";
        for (json const &construction : rule["constructions"]) {
            std::string       construction_name = construction["tag"];
            std::string const construction_id   = "tagged_" + to_upper(construction_name);

            rules_c_stream << "    translate_" << rule_id << "_" << construction_id << ",\n";
        }
        rules_c_stream << "};\n\n";
    }
    // the rule_apply_fn_AST_NODE table
    rules_c_stream << "rule_apply_fn rule_apply_fn_AST_NODE[" << rules.size() + 1
                   << "] = {\n    translate_AST_NODE_SIMPLE_TOKEN,\n";
    for (auto const &rule : rules) {
        std::string const rule_name = rule["name"];
        std::string const rule_id   = "AST_NODE_" + to_upper(rule_name);

        rules_c_stream << "    translate_" << rule_id << ",\n";
    }
    rules_c_stream << "};\n\n";
}

int
main(int argc, char **argv) {

    mc::Cmd_Options options = mc::parse_cmd_options(argc, argv);
    mc::logging::init_logging(options.log_level);

    MC_TRACE_FUNCTION(
        "options: input_file: {}, output_dir: {}, log_level: {}", options.input_file.string(),
        options.output_dir.string(), mc::logging::log_level_to_string(options.log_level)->data());

    auto ld = mc::Language_Description::new_from_json(options.input_file);
    ld.validate_rules();

    json language_description{};

    std::ifstream ifs{options.input_file};
    ifs >> language_description;
    ifs.close();

    init_directory_structure(options.output_dir);
    begin_lexer_parser();
    generate_lexer(language_description);
    generate_parser_options(language_description);
    generate_parser(language_description);
    finalize_lexer_parser();
    generate_ast_h(language_description);
    generate_ast_c(language_description);
    generate_rules_h(language_description);
    generate_rules_c(language_description);
    // generate_makefile(language_description);

    // close all streams
    lexer_stream.close();
    parser_stream.close();
    ast_c_stream.close();
    ast_h_stream.close();
    rules_c_stream.close();
    rules_h_stream.close();
    // makefile_stream.close();

    mc::generate(ld, options.output_dir);
}