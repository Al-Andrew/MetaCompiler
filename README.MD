# Meta Compiler:
Provide a system to create transpilers from any user-defined language to a user-chosen 'backend'.


## Architecture:
----
### Bird's eye view:
```mermaid
stateDiagram-v2
    userLanguageDefinition: LanguageDefinition
    metaCompiler: Meta Compiler
    userLanguageCompiler: Transpiler
    userLanguageCode: Code
    targetLanguageCompiler: TargetLanguage compiler
    transpiledUserLanguageCode: Transpiled Code
    executable: Executable

    userLanguageDefinition --> metaCompiler : feeds into
    metaCompiler --> userLanguageCompiler : generates
    userLanguageCode --> userLanguageCompiler : feeds into
    userLanguageCompiler --> transpiledUserLanguageCode : generates
    transpiledUserLanguageCode --> targetLanguageCompiler : feeds into
    targetLanguageCompiler --> executable : generates
```

### MetaCompiler internals:
```mermaid
stateDiagram-v2
    userLanguageDefinition: LanguageDefinition
    transpiler: Transpiler
    state MetaCompiler {
        [*] --> Syntax : contains
        [*] --> Semantics : contains
        Syntax --> ParserGenerator : feeds into
        ParserGenerator --> Parser : generates
        Semantics --> CodeGenerator : is used to create
        Parser --> [*] : combine into
        CodeGenerator --> [*] : combine into
    }
    userLanguageDefinition --> MetaCompiler : feeds into
    MetaCompiler --> transpiler : generates
```

### Transpiler internals:
```mermaid
stateDiagram-v2
    code: Code
    state Transpiler {
        [*] --> Parser : feeds into
        Parser --> AST : parses
        AST --> CodeGenerator : feeds into
        CodeGenerator --> [*] : generates
    }

    code --> Transpiler
    Transpiler --> TranspiledCode : generates
```