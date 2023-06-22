

#include <stdio.h>
#include "ast.hpp"
#include "tokens.hpp"
#include "lexer.hpp"
#include <fstream>
#include <unordered_map>

extern Ast_Node* ast_root;
extern void yyparse();
extern std::ofstream out_stream;

int main(int argc, char** argv){
    if(argc < 3) { 
        printf("Usage: %s <input_file> <output_file>", argv[0]);
        return 1;
    }
    
    yyin=fopen(argv[1],"r");
    yyparse();
    printf("Parsing complete.\n\n");
    
    ast_root->print();
    out_stream.open(argv[2]);
    ast_root->traverse();
    
    return 0;
}

