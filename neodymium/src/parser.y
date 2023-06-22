
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

%token <ast_node>TKN_ID_LPAREN
%token <ast_node>TKN_ID_RPAREN
%token <ast_node>TKN_ID_KEYWORD_LANG
%token <ast_node>TKN_ID_KEYWORD_TOKENS
%token <ast_node>TKN_ID_KEYWORD_TEXT
%token <ast_node>TKN_ID_KEYWORD_REGEX
%token <ast_node>TKN_ID_KEYWORD_RULES
%token <ast_node>TKN_ID_KEYWORD_CONSTRUCTIONS
%token <ast_node>TKN_ID_KEYWORKD_SYMBOLS
%token <ast_node>TKN_ID_KEYWORD_ACTION
%token <ast_node>TKN_ID_KEYWORD_START
%token <ast_node>TKN_ID_KEYWORD_MAIN
%token <ast_node>TKN_ID_KEYWORD_COMPONENT
%token <ast_node>TKN_ID_KEYWORD_INCLUDE
%token <ast_node>TKN_ID_LITERAL_STRING
%token <ast_node>TKN_ID_LITERAL_CPP
%token <ast_node>TKN_ID_COMPONENT_ACTION
%token <ast_node>TKN_ID_COMPONENT_ACTION_PARAM

%type <ast_node>file
%type <ast_node>token
%type <ast_node>tokens_list
%type <ast_node>tokens_def
%type <ast_node>ca_param
%type <ast_node>component_action_param_list
%type <ast_node>action_expand
%type <ast_node>action_expand_list
%type <ast_node>action_def
%type <ast_node>symbols_list
%type <ast_node>symbols_def
%type <ast_node>construction
%type <ast_node>constructions_list
%type <ast_node>constructions_def
%type <ast_node>rule
%type <ast_node>rules_list
%type <ast_node>rules_def
%type <ast_node>main_def
%type <ast_node>component_action
%type <ast_node>component_action_list
%type <ast_node>include_list
%type <ast_node>include_paths_list


%start file
%%

file : TKN_ID_LPAREN TKN_ID_KEYWORD_LANG TKN_ID_LITERAL_STRING tokens_def rules_def main_def TKN_ID_RPAREN { $$ = Ast_Node_Construction_FILE_LANG::make($1, $2, $3, $4, $5, $6, $7); ast_root = $$; }
| TKN_ID_LPAREN TKN_ID_KEYWORD_LANG TKN_ID_LITERAL_STRING include_list tokens_def rules_def main_def TKN_ID_RPAREN { $$ = Ast_Node_Construction_FILE_LANG_WITH_INCLUDES::make($1, $2, $3, $4, $5, $6, $7, $8); ast_root = $$; }
| TKN_ID_LPAREN TKN_ID_KEYWORD_COMPONENT TKN_ID_LITERAL_STRING component_action_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_FILE_COMPONENT::make($1, $2, $3, $4, $5); ast_root = $$; }
token : TKN_ID_LPAREN TKN_ID_KEYWORD_TEXT TKN_ID_LITERAL_STRING TKN_ID_LITERAL_STRING TKN_ID_RPAREN { $$ = Ast_Node_Construction_TOKEN_TEXT::make($1, $2, $3, $4, $5);}
| TKN_ID_LPAREN TKN_ID_KEYWORD_REGEX TKN_ID_LITERAL_STRING TKN_ID_LITERAL_STRING TKN_ID_RPAREN { $$ = Ast_Node_Construction_TOKEN_REGEX::make($1, $2, $3, $4, $5);}
tokens_list : token { $$ = Ast_Node_Construction_TOKENS_LIST_BASE::make($1);}
| token tokens_list { $$ = Ast_Node_Construction_TOKENS_LIST_RECURSIVE::make($1, $2);}
tokens_def : TKN_ID_LPAREN TKN_ID_KEYWORD_TOKENS tokens_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_TOKENS_DEF_BASE::make($1, $2, $3, $4);}
ca_param : TKN_ID_COMPONENT_ACTION_PARAM { $$ = Ast_Node_Construction_CA_PARAM_CA::make($1);}
| TKN_ID_LITERAL_CPP { $$ = Ast_Node_Construction_CA_PARAM_CPP::make($1);}
component_action_param_list : ca_param { $$ = Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA::make($1);}
| ca_param component_action_param_list { $$ = Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE::make($1, $2);}
action_expand : TKN_ID_LITERAL_CPP { $$ = Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL::make($1);}
| TKN_ID_LPAREN TKN_ID_COMPONENT_ACTION component_action_param_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS::make($1, $2, $3, $4);}
| TKN_ID_LPAREN TKN_ID_COMPONENT_ACTION TKN_ID_RPAREN { $$ = Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS::make($1, $2, $3);}
action_expand_list : action_expand { $$ = Ast_Node_Construction_ACTION_EXPAND_LIST_BASE::make($1);}
| action_expand action_expand_list { $$ = Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE::make($1, $2);}
action_def : TKN_ID_LPAREN TKN_ID_KEYWORD_ACTION action_expand_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_ACTION_DEF_BASE::make($1, $2, $3, $4);}
symbols_list : TKN_ID_LITERAL_STRING { $$ = Ast_Node_Construction_SYMBOLS_LIST_BASE::make($1);}
| TKN_ID_LITERAL_STRING symbols_list { $$ = Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE::make($1, $2);}
symbols_def : TKN_ID_LPAREN TKN_ID_KEYWORKD_SYMBOLS symbols_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_SYMBOLS_DEF_BASE::make($1, $2, $3, $4);}
construction : TKN_ID_LPAREN TKN_ID_LITERAL_STRING symbols_def action_def TKN_ID_RPAREN { $$ = Ast_Node_Construction_CONSTRUCTION_BASE::make($1, $2, $3, $4, $5);}
constructions_list : construction { $$ = Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE::make($1);}
| construction constructions_list { $$ = Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE::make($1, $2);}
constructions_def : TKN_ID_LPAREN TKN_ID_KEYWORD_CONSTRUCTIONS constructions_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE::make($1, $2, $3, $4);}
rule : TKN_ID_LPAREN TKN_ID_LITERAL_STRING constructions_def TKN_ID_RPAREN { $$ = Ast_Node_Construction_RULE_BASE::make($1, $2, $3, $4);}
| TKN_ID_LPAREN TKN_ID_LITERAL_STRING TKN_ID_LPAREN TKN_ID_KEYWORD_START TKN_ID_RPAREN constructions_def TKN_ID_RPAREN { $$ = Ast_Node_Construction_RULE_WITH_START::make($1, $2, $3, $4, $5, $6, $7);}
rules_list : rule { $$ = Ast_Node_Construction_RULES_LIST_BASE::make($1);}
| rule rules_list { $$ = Ast_Node_Construction_RULES_LIST_RECURSIVE::make($1, $2);}
rules_def : TKN_ID_LPAREN TKN_ID_KEYWORD_RULES rules_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_RULES_DEF_BASE::make($1, $2, $3, $4);}
main_def : TKN_ID_LPAREN TKN_ID_KEYWORD_MAIN action_expand_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_MAIN_DEF_BASE::make($1, $2, $3, $4);}
component_action : TKN_ID_LPAREN TKN_ID_KEYWORD_ACTION TKN_ID_LITERAL_STRING TKN_ID_LITERAL_CPP TKN_ID_RPAREN { $$ = Ast_Node_Construction_COMPONENT_ACTION_BASE::make($1, $2, $3, $4, $5);}
component_action_list : component_action { $$ = Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE::make($1);}
| component_action component_action_list { $$ = Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE::make($1, $2);}
include_list : TKN_ID_LPAREN TKN_ID_KEYWORD_INCLUDE include_paths_list TKN_ID_RPAREN { $$ = Ast_Node_Construction_INCLUDE_LIST_BASE::make($1, $2, $3, $4);}
include_paths_list : TKN_ID_LITERAL_STRING { $$ = Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE::make($1);}
| TKN_ID_LITERAL_STRING include_paths_list { $$ = Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE::make($1, $2);}


%%
int yyerror(const char * s) {

    printf("[Line: %d][Col: %u] Error: %s. Arround %s\n", yylineno, column_number, s, yytext);
    yylineno = 1;
    column_number = 1;
    return 1;
}
