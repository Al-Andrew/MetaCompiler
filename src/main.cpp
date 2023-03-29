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


std::string to_upper(std::string& str) {
    std::string result;
    for (char& c : str) {
        result = std::toupper(c);
    }
    return result;
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

void init_directory_structure() {
    std::filesystem::create_directory("generated");
    lexer_stream.open("generated/lexer.l");
    parser_stream.open("generated/parser.y");

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
%}
%option noyywrap
%%

)";

    parser_stream << R"(
%{
#include <stdio.h>

extern FILE* yyin;
extern char* yytext;
extern int yylineno;
%}

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
        parser_stream << "%token " << token_id << '\n';
        lexer_stream << (is_regex?"": "\"") << token_matcher << (is_regex?"": "\"") << " { printf(\" lexing: " << token_id <<  "\\n\"); return " << token_id << "; }\n";

        LOG_INFO << "writing token: " << token_id << " | matcher: " << token_matcher << '\n';
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
        std::string const rule_name = rule["name"];
        std::vector<json> const constructions = rule["constructions"];

        parser_stream << rule_name << " : ";
        LOG_INFO << "writing rules for grammar piece: " << rule_name << '\n';
        
        auto write_construction = [&](json const& construction) {
            std::string const tag = construction["tag"];
            std::vector<std::string> const symbols = construction["symbols"];

            for(auto const& symbol : symbols) {
                std::string possible_token_id = TokenList::symbol_name_to_token_id(symbol);
                if (TokenList::the()->is_valid_token(symbol)) {
                    parser_stream << possible_token_id << ' ';
                } else {
                    parser_stream << symbol << ' ';
                }
            }
            parser_stream << "{ printf(\"parsing: " << tag << "\\n\"); }";
            parser_stream << "\n";
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
}
)";

    LOG_INFO << "Finalizing lexer + parser.\n";
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
}