
#pragma once
#include <string>

enum class Token_Type : unsigned int {
    TKN_ID_LPAREN,
    TKN_ID_RPAREN,
    TKN_ID_KEYWORD_LANG,
    TKN_ID_KEYWORD_TOKENS,
    TKN_ID_KEYWORD_TEXT,
    TKN_ID_KEYWORD_REGEX,
    TKN_ID_KEYWORD_RULES,
    TKN_ID_KEYWORD_CONSTRUCTIONS,
    TKN_ID_KEYWORKD_SYMBOLS,
    TKN_ID_KEYWORD_ACTION,
    TKN_ID_KEYWORD_START,
    TKN_ID_KEYWORD_MAIN,
    TKN_ID_KEYWORD_COMPONENT,
    TKN_ID_KEYWORD_INCLUDE,
    TKN_ID_LITERAL_STRING,
    TKN_ID_LITERAL_CPP,
    TKN_ID_COMPONENT_ACTION,
    TKN_ID_COMPONENT_ACTION_PARAM,

};

struct Token {
    Token_Type type;
    std::string value;
    unsigned int line;
    unsigned int column;
};

