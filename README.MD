# Meta Compiler:
Provide a system to create transpilers from any user-defined language to a user-chosen 'backend'.


## Architecture:
----

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