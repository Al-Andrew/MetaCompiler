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
static std::ofstream praser_stream;

void init_directory_structure() {
    std::filesystem::create_directory("generated");
    lexer_stream.open("generated/lexer.l");
    praser_stream.open("generated/parser.y");
    printf("Created directory structure\n");
    lexer_stream << R"(
%{

%}
%option noyywrap
%%

)";

    praser_stream << R"(
%{

%}

)";
}


void generate_lexer(json const& language_description) {
    std::vector<json> const tokens = language_description["tokens"];


    for (json const& token : tokens) {
        std::string const symbol_name = token["name"];
        std::string const token_regex = token["regex"];
        std::string const token_id = TokenList::symbol_name_to_token_id(symbol_name);
        TokenList::the()->add_token(token_id);
        praser_stream << "%token " << token_id << '\n';
        lexer_stream << '"' << token_regex << '"' << " { return " << token_id << "; }\n";
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
    generate_lexer(language_description);
}