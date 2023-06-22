
%{
#include "tokens.hpp"
#include "ast.hpp"
#include "parser.hpp"

unsigned int column_number = 1;
%}

%option noyywrap

%%

"(" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_LPAREN, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_LPAREN; }
")" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_RPAREN, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_RPAREN; }
"lang" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_LANG, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_LANG; }
"tokens" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_TOKENS, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_TOKENS; }
"text" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_TEXT, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_TEXT; }
"regex" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_REGEX, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_REGEX; }
"rules" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_RULES, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_RULES; }
"constructions" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_CONSTRUCTIONS, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_CONSTRUCTIONS; }
"symbols" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORKD_SYMBOLS, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORKD_SYMBOLS; }
"action" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_ACTION, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_ACTION; }
"start" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_START, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_START; }
"main" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_MAIN, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_MAIN; }
"component" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_COMPONENT, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_COMPONENT; }
"include" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_KEYWORD_INCLUDE, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_KEYWORD_INCLUDE; }
\"([^"\\]|\\.)*\" { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_LITERAL_STRING, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_LITERAL_STRING; }
\`[^\`]*\` { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_LITERAL_CPP, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_LITERAL_CPP; }
[a-zA-Z0-9_]+.[a-zA-Z0-9_]+ { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_COMPONENT_ACTION, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_COMPONENT_ACTION; }
\$[0-9][1-9]* { column_number += yyleng; yylval.ast_node = Ast_Node_Token::make(Token{Token_Type::TKN_ID_COMPONENT_ACTION_PARAM, yytext, (unsigned int)yylineno, column_number}); return TKN_ID_COMPONENT_ACTION_PARAM; }


[ \t] { column_number++; }
\n {yylineno++; column_number = 1;}
"//".*$ {;}
. { column_number++; return yytext[0];}

%%
