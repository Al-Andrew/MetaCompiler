```mermaid
flowchart LR
    code["cod sursă"]
    tokens["șir de tokeni"]
    ast["arbore de sintaxă"]
    aast["arbore de sintaxă adnotat"]

    code --> tokens
    tokens --> ast
    ast --> aast
```

```mermaid
graph TD
    = --> x
    = --> +
    + --> 5
    + --> 2
```

```mermaid
flowchart LR
    cod["cod sursă"]
    comp["compiler"]
    exe["program executabil"]
    in["date de intrare"]
    out["date de ieșire"]
    cod --> comp
    comp --> exe
    in --> exe
    exe --> out
```

```mermaid
flowchart LR
    cod["cod sursă"]
    int["interpretor"]
    in["date de intrare"]
    out["date de ieșire"]
    cod --> int
    in --> int
    int --> out
```

```mermaid
flowchart LR
    cod["cod sursă"]
    comp["compilator"]
    vm["mașină virtuală"]
    exe["instrucțiuni vm"]
    in["date de intrare"]
    out["date de ieșire"]
    cod --> comp
    comp --> exe
    exe --> vm
    in --> vm
    vm --> out
```

```mermaid
flowchart LR
    ld["LanguageDefinition"]
    mc["MetaCompiler"]
    ulc["Programming Language"]

    ld --> mc
    mc --> ulc
```
```mermaid
stateDiagram-v2
    ana: Ana\nsubstantiv
    are: are\n verb
    mere: mere\n substantiv
    punct: .\n punct
```
```mermaid
stateDiagram-v2
    let: let\ncuvânt cheie
    x: x\nidentificator
    eq: =\noperator =
    four: 4\nliteral întreg
```
```mermaid
flowchart TD
    et1["etc."]
    et2["etc."]
    Odin --> LLVM
    Zig --> LLVM
    Rust --> LLVM
    C --> LLVM
    C++ --> LLVM
    et1 --> LLVM
    LLVM --> x86_64
    LLVM --> ARM
    LLVM --> MIPS
    LLVM --> et2

```

 {
    state "Tokeni valizi" as tokens
    state "Reguli de sintaxă" as syntax
    state "Semantica regulilor de sintaxă" as semantics
    [*] --> tokens : conține
    [*] --> syntax : conține
    [*] --> semantics : conține
}

```mermaid
stateDiagram-v2
    state "Descriere a limbajului" as desc
    state "MetaCompiler" as mc {
        state "Neodymium" as neodymium
        state "Reprezentare intermediară JSON" as json {
            state "Tokeni valizi" as tokens
            state "Reguli de sintaxă" as syntax
            state "Semantica regulilor de sintaxă" as semantics
        }
        state "Șablon" as stencil {
            state "lexer.l" as lexer
            state "parser.y" as parser
            state "ast.cpp" as generator
        }
        [*] --> neodymium
        neodymium --> json
        
        stencil --> [*]
        json --> [*]
    }
    state "Noul limbaj de programare" as nlp

    desc --> mc
    mc --> nlp
```