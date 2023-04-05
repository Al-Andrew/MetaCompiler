#include <cctype>
#include <cstdio>
#include <map>
#include <memory>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <set>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#define LOG_INFO std::clog << "[INFO] [" << __FUNCTION__ << ':' << __LINE__ << "] "


std::string to_upper(std::string const& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}

std::string rule_name_and_tag_to_tagid(std::string const& rule_name, std::string const& tag) {
    return std::string{"TAG_ID_"} + to_upper(rule_name) + "_" + to_upper(tag);
}

struct TokenList {
    std::set<std::string> valid_tokens;
    static std::shared_ptr<TokenList> instance;

    static std::shared_ptr<TokenList> the() {
        return instance;
    }

    void add_token(std::string const& token) {
        valid_tokens.insert(token);
    }

    bool is_valid_token(std::string const& token) {
        return valid_tokens.find(token) != valid_tokens.end();
    }

    static std::string symbol_name_to_token_id(std::string const& symbol_name) {
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

void init_directory_structure() {
    std::filesystem::create_directory("generated");
    lexer_stream.open("generated/lexer.l");
    parser_stream.open("generated/parser.y");
    ast_c_stream.open("generated/Ast.c");
    ast_h_stream.open("generated/Ast.h");
    rules_c_stream.open("generated/Rules.c");
    rules_h_stream.open("generated/Rules.h");

    try {
        std::filesystem::copy_file("templates/Makefile", "generated/Makefile");
    } catch (std::filesystem::filesystem_error const& e) {
        LOG_INFO << "Failed to copy Makefile: " << e.what() << '\n';
    }

    LOG_INFO << "Created directory structure.\n";

}


void begin_lexer_parser() {
        lexer_stream << R"(
%{
#include "y.tab.h"
#include "Ast.h"
%}
%option noyywrap
%%

)";

    parser_stream << R"(
%{
#include <stdio.h>
#include "Ast.h"

extern FILE* yyin;
extern char* yytext;
extern int yylineno;
%}

%union {
    struct _Ast_Node* node;
}

)";
    
    LOG_INFO << "Beginning lexer + parser.\n";
}

void generate_lexer(json const& language_description) {
    std::vector<json> const tokens = language_description["tokens"];

    for (json const& token : tokens) {
        std::string const symbol_name = token["name"];
        std::string const token_matcher = token["matcher"];
        bool const is_regex = token["is_regex"].get<bool>();

        std::string const token_id = TokenList::symbol_name_to_token_id(symbol_name);
        TokenList::the()->add_token(symbol_name);
        parser_stream << "%token <node>" << token_id << '\n';
        lexer_stream << (is_regex?"": "\"") << token_matcher << (is_regex?"": "\"") << " { printf(\" lexing: " << token_id <<  "\\n\");" 
         << " yylval.node = ast_node_new_token(\"" << token_id << "\", yytext);"
         << " return " << token_id << "; }\n";

        LOG_INFO << "writing token: " << token_id << " | matcher: " << token_matcher << '\n';
    }

    std::vector<json> const rules = language_description["rules"];
    for(json const& rule : rules) {
        std::string const rule_name = rule["name"];

        parser_stream << "%type <node>" << rule_name << '\n';
    }

    LOG_INFO << "Tokens writen to lexer + parser.\n";
}

void generate_parser_options(json const& language_description) {
    parser_stream << R"(

%start )" << language_description["start_rule"].get<std::string>() << R"(

%%

)";
}

void generate_parser(json const& language_description) {
    std::vector<json> const rules = language_description["rules"];

    for (json const& rule : rules) {
        std::string rule_name = rule["name"];
        std::vector<json> const constructions = rule["constructions"];

        parser_stream << rule_name << " : ";
        LOG_INFO << "writing rules for grammar piece: " << rule_name << '\n';
        
        auto write_construction = [&](json const& construction) {
            std::string const tag = construction["tag"];
            std::string const tag_id = rule_name_and_tag_to_tagid(rule_name, tag);
            std::vector<std::string> const symbols = construction["symbols"];

            for(auto const& symbol : symbols) {
                std::string possible_token_id = TokenList::symbol_name_to_token_id(symbol);
                if (TokenList::the()->is_valid_token(symbol)) {
                    parser_stream << possible_token_id << ' ';
                } else {
                    parser_stream << symbol << ' ';
                }
            }
            parser_stream << "{ printf(\"parsing: " << tag_id << "\\n\"); "
                << "$$ = ast_node_new(\"" << tag_id << "\", " << "AST_NODE_" << to_upper(rule_name) << ", " << tag_id << ", " << symbols.size() << ", ";
            for (size_t idx = 0; idx < symbols.size(); ++idx) {
                parser_stream << "$" << idx + 1;
                if (idx != symbols.size() - 1) {
                    parser_stream << ", ";
                }
            }

            parser_stream << ");";
            
            if(rule_name == language_description["start_rule"].get<std::string>()) {
                parser_stream << "ast_root = $$;";
            }

            parser_stream << "}\n";
        };

        write_construction(constructions[0]);        
        for (size_t idx = 1; idx < constructions.size(); ++idx) {
            json const& construction = constructions[idx];
            parser_stream << "| ";
            write_construction(construction);
        }
        parser_stream << ";\n\n";
    }
}


void finalize_lexer_parser() {
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
    yyin=fopen(argv[1],"r");
    yyparse();
    printf("Parsing complete.\n\n");
    ast_node_print(ast_root, 0);
}
)";

    LOG_INFO << "Finalizing lexer + parser.\n";
}

void generate_ast_h(json const& language_description) {
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

    for (json const& rule : rules) {
        std::string  rule_name = rule["name"];
        std::string const rule_id = "AST_NODE_" + to_upper(rule_name);
        ast_h_stream << "    " << rule_id << ",\n";
    }
    ast_h_stream << "} Ast_Node_Type;\n\n";
    // ================================

    // Note(AAL): generate one enum for each rule containg its tags
    for (json const& rule : rules) {
        std::string  rule_name = rule["name"];
        std::string const rule_id = "AST_NODE_" + to_upper(rule_name);
        std::vector<json> const constructions = rule["constructions"];

        ast_h_stream << "typedef enum _" << rule_id << "_Tag {\n";
        for (json const& construction : constructions) {
            std::string const tag = construction["tag"];
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
    const char* value;

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

void generate_ast_c(json const& language_description) {
    ast_c_stream << R"(
#include "Ast.h"
#include "temp/y.tab.h"

#include "string.h"
#include "stdio.h"  

Ast_Node* ast_root = NULL;

Ast_Node* ast_node_new(const char* name, uint32_t type, uint32_t tag, uint32_t num_children, ...) {
    Ast_Node* node = (Ast_Node*)malloc(sizeof(Ast_Node));
    node->name = name;
    node->type = type;
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
    node->value = strdup(value);
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
    for (json const& token : tokens) {
        std::string const token_name = token["name"];
        std::string const token_id = TokenList::symbol_name_to_token_id(token_name);
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
    for (json const& rule : rules) {
        std::string rule_name = rule["name"];
        std::string const rule_id = "AST_NODE_" + to_upper(rule_name);
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

void generate_rules_h(json const& language_description) {
    std::vector<json> const rules = language_description["rules"];

    rules_h_stream << R"(
#ifndef _RULES_H_
#define _RULES_H_

#include "Ast.h"
#include "stdio.h"

typedef void (*rule_apply_fn)(FILE* stream, Ast_Node* node);

void translate_AST_NODE_SIMPLE_TOKEN(FILE* stream, Ast_Node* node);

void translate_AST_NODE(FILE* stream, Ast_Node* node);
extern rule_apply_fn rule_apply_fn_AST_NODE[)" << rules.size() << "];\n\n";

    for (json const& rule : rules) {
        std::string rule_name = rule["name"];
        std::string const rule_id = "AST_NODE_" + to_upper(rule_name);
        
        rules_h_stream << "void translate_" << rule_id << "(FILE* stream, Ast_Node* node);\n";
        for (json const& construction : rule["constructions"]) {
            std::string construction_name = construction["tag"];
            std::string const construction_id = "tagged_" + to_upper(construction_name);

            rules_h_stream << "void translate_" << rule_id << "_" << construction_id << "(FILE* stream, Ast_Node* node);\n";
        }

        rules_h_stream << "extern rule_apply_fn rule_apply_fn_" << rule_id << "[" << rule["constructions"].size() << "];\n\n";
    }
    rules_h_stream << R"(


#endif //! _RULES_H_
)";

}

void generate_rules_c(json const& language_description) {
    std::vector<json> const rules = language_description["rules"];

    rules_c_stream << R"(
#include "Rules.h"

void translate_AST_NODE_SIMPLE_TOKEN(FILE* stream, Ast_Node* node) {
    fprintf(stream, "%s", node->value);
}

void translate_AST_NODE(FILE* stream, Ast_Node* node) {
    rule_apply_fn fn = rule_apply_fn_AST_NODE[node->type];
    fn(stream, node);
}

)";

    // generate the function definitions
    for(auto const& rule : rules ) {
        std::string const rule_name = rule["name"];
        std::string const rule_id = "AST_NODE_" + to_upper(rule_name);

        // the function for this rule
        rules_c_stream << "void translate_" << rule_id << "(FILE* stream, Ast_Node* node) {\n";
        rules_c_stream << "    rule_apply_fn fn = rule_apply_fn_" << rule_id << "[node->tag];\n";
        rules_c_stream << "    fn(stream, node);\n";
        rules_c_stream << "}\n\n";

        // the functions for this rule's tags
        for (json const& construction : rule["constructions"]) {
            std::string construction_name = construction["tag"];
            std::string const construction_id = "tagged_" + to_upper(construction_name);

            rules_c_stream << "void translate_" << rule_id << "_" << construction_id << "(FILE* stream, Ast_Node* node) {\n";
            // TODO(AAL): parse out the translation out of the construction and generate the code here
            rules_c_stream << "}\n\n";
        }

        // the table for this rule's tags
        rules_c_stream << "rule_apply_fn rule_apply_fn_" << rule_id << "[" << rule["constructions"].size() << "] = {\n";
        for (json const& construction : rule["constructions"]) {
            std::string construction_name = construction["tag"];
            std::string const construction_id = "tagged_" + to_upper(construction_name);

            rules_c_stream << "    translate_" << rule_id << "_" << construction_id << ",\n";
        }
        rules_c_stream << "};\n\n";
    }

}


int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    json language_description{}; 
    
    std::ifstream ifs{"simple_ld.json"};
    ifs >> language_description;
    ifs.close();

    init_directory_structure();
    begin_lexer_parser();
    generate_lexer(language_description);
    generate_parser_options(language_description);
    generate_parser(language_description);
    finalize_lexer_parser();
    generate_ast_h(language_description);
    generate_ast_c(language_description);
    generate_rules_h(language_description);
    generate_rules_c(language_description);
}