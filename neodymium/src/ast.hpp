
#pragma once
#include "tokens.hpp"

#include <vector>
#include <iostream>

struct Ast_Node {
    std::vector<Ast_Node*> children;

    Ast_Node(std::vector<Ast_Node*> children) : children(children) {}
    virtual ~Ast_Node();

    virtual const char* get_name() = 0;

    virtual void print(unsigned int indent = 0) {
        std::string indent_str(indent, ' ');
        std::cout << indent_str << "-" << get_name() << "\n";
        for (auto child : children) {
            child->print(indent + 1);
        }
    }

    virtual void traverse() = 0;
};

struct Ast_Node_Token : public Ast_Node {
    Token token;

    Ast_Node_Token(std::vector<Ast_Node*> children, Token token) : Ast_Node(children), token(token) {}

    static Ast_Node_Token* make(Token token) {
        return new Ast_Node_Token{std::vector<Ast_Node*>{}, token};
    }

    virtual const char* get_name() override {
        return "Ast_Node_Token";
    }

    virtual void print(unsigned int indent = 0) override {
        std::string indent_str(indent, ' ');
        std::cout << indent_str << "-" << get_name() << " : " << token.value << "\n";
    }

    virtual void traverse() override {}
};

struct Ast_Node_FILE : public Ast_Node {
    Ast_Node_FILE(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_FILE();
};
struct Ast_Node_Construction_FILE_LANG : public Ast_Node_FILE {
    Ast_Node_Construction_FILE_LANG(std::vector<Ast_Node*> children) : Ast_Node_FILE(children) {}

    virtual ~Ast_Node_Construction_FILE_LANG();

    static Ast_Node_Construction_FILE_LANG* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4, Ast_Node* p5, Ast_Node* p6);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_FILE_LANG_WITH_INCLUDES : public Ast_Node_FILE {
    Ast_Node_Construction_FILE_LANG_WITH_INCLUDES(std::vector<Ast_Node*> children) : Ast_Node_FILE(children) {}

    virtual ~Ast_Node_Construction_FILE_LANG_WITH_INCLUDES();

    static Ast_Node_Construction_FILE_LANG_WITH_INCLUDES* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4, Ast_Node* p5, Ast_Node* p6, Ast_Node* p7);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_FILE_COMPONENT : public Ast_Node_FILE {
    Ast_Node_Construction_FILE_COMPONENT(std::vector<Ast_Node*> children) : Ast_Node_FILE(children) {}

    virtual ~Ast_Node_Construction_FILE_COMPONENT();

    static Ast_Node_Construction_FILE_COMPONENT* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_TOKEN : public Ast_Node {
    Ast_Node_TOKEN(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_TOKEN();
};
struct Ast_Node_Construction_TOKEN_TEXT : public Ast_Node_TOKEN {
    Ast_Node_Construction_TOKEN_TEXT(std::vector<Ast_Node*> children) : Ast_Node_TOKEN(children) {}

    virtual ~Ast_Node_Construction_TOKEN_TEXT();

    static Ast_Node_Construction_TOKEN_TEXT* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_TOKEN_REGEX : public Ast_Node_TOKEN {
    Ast_Node_Construction_TOKEN_REGEX(std::vector<Ast_Node*> children) : Ast_Node_TOKEN(children) {}

    virtual ~Ast_Node_Construction_TOKEN_REGEX();

    static Ast_Node_Construction_TOKEN_REGEX* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_TOKENS_LIST : public Ast_Node {
    Ast_Node_TOKENS_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_TOKENS_LIST();
};
struct Ast_Node_Construction_TOKENS_LIST_BASE : public Ast_Node_TOKENS_LIST {
    Ast_Node_Construction_TOKENS_LIST_BASE(std::vector<Ast_Node*> children) : Ast_Node_TOKENS_LIST(children) {}

    virtual ~Ast_Node_Construction_TOKENS_LIST_BASE();

    static Ast_Node_Construction_TOKENS_LIST_BASE* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_TOKENS_LIST_RECURSIVE : public Ast_Node_TOKENS_LIST {
    Ast_Node_Construction_TOKENS_LIST_RECURSIVE(std::vector<Ast_Node*> children) : Ast_Node_TOKENS_LIST(children) {}

    virtual ~Ast_Node_Construction_TOKENS_LIST_RECURSIVE();

    static Ast_Node_Construction_TOKENS_LIST_RECURSIVE* make(Ast_Node* p0, Ast_Node* p1);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_TOKENS_DEF : public Ast_Node {
    Ast_Node_TOKENS_DEF(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_TOKENS_DEF();
};
struct Ast_Node_Construction_TOKENS_DEF_BASE : public Ast_Node_TOKENS_DEF {
    Ast_Node_Construction_TOKENS_DEF_BASE(std::vector<Ast_Node*> children) : Ast_Node_TOKENS_DEF(children) {}

    virtual ~Ast_Node_Construction_TOKENS_DEF_BASE();

    static Ast_Node_Construction_TOKENS_DEF_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_CA_PARAM : public Ast_Node {
    Ast_Node_CA_PARAM(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_CA_PARAM();
};
struct Ast_Node_Construction_CA_PARAM_CA : public Ast_Node_CA_PARAM {
    Ast_Node_Construction_CA_PARAM_CA(std::vector<Ast_Node*> children) : Ast_Node_CA_PARAM(children) {}

    virtual ~Ast_Node_Construction_CA_PARAM_CA();

    static Ast_Node_Construction_CA_PARAM_CA* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_CA_PARAM_CPP : public Ast_Node_CA_PARAM {
    Ast_Node_Construction_CA_PARAM_CPP(std::vector<Ast_Node*> children) : Ast_Node_CA_PARAM(children) {}

    virtual ~Ast_Node_Construction_CA_PARAM_CPP();

    static Ast_Node_Construction_CA_PARAM_CPP* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_COMPONENT_ACTION_PARAM_LIST : public Ast_Node {
    Ast_Node_COMPONENT_ACTION_PARAM_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_COMPONENT_ACTION_PARAM_LIST();
};
struct Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA : public Ast_Node_COMPONENT_ACTION_PARAM_LIST {
    Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA(std::vector<Ast_Node*> children) : Ast_Node_COMPONENT_ACTION_PARAM_LIST(children) {}

    virtual ~Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA();

    static Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE : public Ast_Node_COMPONENT_ACTION_PARAM_LIST {
    Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE(std::vector<Ast_Node*> children) : Ast_Node_COMPONENT_ACTION_PARAM_LIST(children) {}

    virtual ~Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE();

    static Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE* make(Ast_Node* p0, Ast_Node* p1);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_ACTION_EXPAND : public Ast_Node {
    Ast_Node_ACTION_EXPAND(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_ACTION_EXPAND();
};
struct Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL : public Ast_Node_ACTION_EXPAND {
    Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL(std::vector<Ast_Node*> children) : Ast_Node_ACTION_EXPAND(children) {}

    virtual ~Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL();

    static Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION : public Ast_Node_ACTION_EXPAND {
    Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION(std::vector<Ast_Node*> children) : Ast_Node_ACTION_EXPAND(children) {}

    virtual ~Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION();

    static Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_ACTION_EXPAND_LIST : public Ast_Node {
    Ast_Node_ACTION_EXPAND_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_ACTION_EXPAND_LIST();
};
struct Ast_Node_Construction_ACTION_EXPAND_LIST_BASE : public Ast_Node_ACTION_EXPAND_LIST {
    Ast_Node_Construction_ACTION_EXPAND_LIST_BASE(std::vector<Ast_Node*> children) : Ast_Node_ACTION_EXPAND_LIST(children) {}

    virtual ~Ast_Node_Construction_ACTION_EXPAND_LIST_BASE();

    static Ast_Node_Construction_ACTION_EXPAND_LIST_BASE* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE : public Ast_Node_ACTION_EXPAND_LIST {
    Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE(std::vector<Ast_Node*> children) : Ast_Node_ACTION_EXPAND_LIST(children) {}

    virtual ~Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE();

    static Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE* make(Ast_Node* p0, Ast_Node* p1);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_ACTION_DEF : public Ast_Node {
    Ast_Node_ACTION_DEF(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_ACTION_DEF();
};
struct Ast_Node_Construction_ACTION_DEF_BASE : public Ast_Node_ACTION_DEF {
    Ast_Node_Construction_ACTION_DEF_BASE(std::vector<Ast_Node*> children) : Ast_Node_ACTION_DEF(children) {}

    virtual ~Ast_Node_Construction_ACTION_DEF_BASE();

    static Ast_Node_Construction_ACTION_DEF_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_SYMBOLS_LIST : public Ast_Node {
    Ast_Node_SYMBOLS_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_SYMBOLS_LIST();
};
struct Ast_Node_Construction_SYMBOLS_LIST_BASE : public Ast_Node_SYMBOLS_LIST {
    Ast_Node_Construction_SYMBOLS_LIST_BASE(std::vector<Ast_Node*> children) : Ast_Node_SYMBOLS_LIST(children) {}

    virtual ~Ast_Node_Construction_SYMBOLS_LIST_BASE();

    static Ast_Node_Construction_SYMBOLS_LIST_BASE* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE : public Ast_Node_SYMBOLS_LIST {
    Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE(std::vector<Ast_Node*> children) : Ast_Node_SYMBOLS_LIST(children) {}

    virtual ~Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE();

    static Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE* make(Ast_Node* p0, Ast_Node* p1);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_SYMBOLS_DEF : public Ast_Node {
    Ast_Node_SYMBOLS_DEF(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_SYMBOLS_DEF();
};
struct Ast_Node_Construction_SYMBOLS_DEF_BASE : public Ast_Node_SYMBOLS_DEF {
    Ast_Node_Construction_SYMBOLS_DEF_BASE(std::vector<Ast_Node*> children) : Ast_Node_SYMBOLS_DEF(children) {}

    virtual ~Ast_Node_Construction_SYMBOLS_DEF_BASE();

    static Ast_Node_Construction_SYMBOLS_DEF_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_CONSTRUCTION : public Ast_Node {
    Ast_Node_CONSTRUCTION(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_CONSTRUCTION();
};
struct Ast_Node_Construction_CONSTRUCTION_BASE : public Ast_Node_CONSTRUCTION {
    Ast_Node_Construction_CONSTRUCTION_BASE(std::vector<Ast_Node*> children) : Ast_Node_CONSTRUCTION(children) {}

    virtual ~Ast_Node_Construction_CONSTRUCTION_BASE();

    static Ast_Node_Construction_CONSTRUCTION_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_CONSTRUCTIONS_LIST : public Ast_Node {
    Ast_Node_CONSTRUCTIONS_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_CONSTRUCTIONS_LIST();
};
struct Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE : public Ast_Node_CONSTRUCTIONS_LIST {
    Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE(std::vector<Ast_Node*> children) : Ast_Node_CONSTRUCTIONS_LIST(children) {}

    virtual ~Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE();

    static Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE : public Ast_Node_CONSTRUCTIONS_LIST {
    Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE(std::vector<Ast_Node*> children) : Ast_Node_CONSTRUCTIONS_LIST(children) {}

    virtual ~Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE();

    static Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE* make(Ast_Node* p0, Ast_Node* p1);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_CONSTRUCTIONS_DEF : public Ast_Node {
    Ast_Node_CONSTRUCTIONS_DEF(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_CONSTRUCTIONS_DEF();
};
struct Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE : public Ast_Node_CONSTRUCTIONS_DEF {
    Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE(std::vector<Ast_Node*> children) : Ast_Node_CONSTRUCTIONS_DEF(children) {}

    virtual ~Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE();

    static Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_RULE : public Ast_Node {
    Ast_Node_RULE(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_RULE();
};
struct Ast_Node_Construction_RULE_BASE : public Ast_Node_RULE {
    Ast_Node_Construction_RULE_BASE(std::vector<Ast_Node*> children) : Ast_Node_RULE(children) {}

    virtual ~Ast_Node_Construction_RULE_BASE();

    static Ast_Node_Construction_RULE_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_RULE_WITH_START : public Ast_Node_RULE {
    Ast_Node_Construction_RULE_WITH_START(std::vector<Ast_Node*> children) : Ast_Node_RULE(children) {}

    virtual ~Ast_Node_Construction_RULE_WITH_START();

    static Ast_Node_Construction_RULE_WITH_START* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4, Ast_Node* p5, Ast_Node* p6);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_RULES_LIST : public Ast_Node {
    Ast_Node_RULES_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_RULES_LIST();
};
struct Ast_Node_Construction_RULES_LIST_BASE : public Ast_Node_RULES_LIST {
    Ast_Node_Construction_RULES_LIST_BASE(std::vector<Ast_Node*> children) : Ast_Node_RULES_LIST(children) {}

    virtual ~Ast_Node_Construction_RULES_LIST_BASE();

    static Ast_Node_Construction_RULES_LIST_BASE* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_RULES_LIST_RECURSIVE : public Ast_Node_RULES_LIST {
    Ast_Node_Construction_RULES_LIST_RECURSIVE(std::vector<Ast_Node*> children) : Ast_Node_RULES_LIST(children) {}

    virtual ~Ast_Node_Construction_RULES_LIST_RECURSIVE();

    static Ast_Node_Construction_RULES_LIST_RECURSIVE* make(Ast_Node* p0, Ast_Node* p1);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_RULES_DEF : public Ast_Node {
    Ast_Node_RULES_DEF(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_RULES_DEF();
};
struct Ast_Node_Construction_RULES_DEF_BASE : public Ast_Node_RULES_DEF {
    Ast_Node_Construction_RULES_DEF_BASE(std::vector<Ast_Node*> children) : Ast_Node_RULES_DEF(children) {}

    virtual ~Ast_Node_Construction_RULES_DEF_BASE();

    static Ast_Node_Construction_RULES_DEF_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_MAIN_DEF : public Ast_Node {
    Ast_Node_MAIN_DEF(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_MAIN_DEF();
};
struct Ast_Node_Construction_MAIN_DEF_BASE : public Ast_Node_MAIN_DEF {
    Ast_Node_Construction_MAIN_DEF_BASE(std::vector<Ast_Node*> children) : Ast_Node_MAIN_DEF(children) {}

    virtual ~Ast_Node_Construction_MAIN_DEF_BASE();

    static Ast_Node_Construction_MAIN_DEF_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_COMPONENT_ACTION : public Ast_Node {
    Ast_Node_COMPONENT_ACTION(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_COMPONENT_ACTION();
};
struct Ast_Node_Construction_COMPONENT_ACTION_BASE : public Ast_Node_COMPONENT_ACTION {
    Ast_Node_Construction_COMPONENT_ACTION_BASE(std::vector<Ast_Node*> children) : Ast_Node_COMPONENT_ACTION(children) {}

    virtual ~Ast_Node_Construction_COMPONENT_ACTION_BASE();

    static Ast_Node_Construction_COMPONENT_ACTION_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_COMPONENT_ACTION_LIST : public Ast_Node {
    Ast_Node_COMPONENT_ACTION_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_COMPONENT_ACTION_LIST();
};
struct Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE : public Ast_Node_COMPONENT_ACTION_LIST {
    Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE(std::vector<Ast_Node*> children) : Ast_Node_COMPONENT_ACTION_LIST(children) {}

    virtual ~Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE();

    static Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE : public Ast_Node_COMPONENT_ACTION_LIST {
    Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE(std::vector<Ast_Node*> children) : Ast_Node_COMPONENT_ACTION_LIST(children) {}

    virtual ~Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE();

    static Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE* make(Ast_Node* p0, Ast_Node* p1);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_INCLUDE_LIST : public Ast_Node {
    Ast_Node_INCLUDE_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_INCLUDE_LIST();
};
struct Ast_Node_Construction_INCLUDE_LIST_BASE : public Ast_Node_INCLUDE_LIST {
    Ast_Node_Construction_INCLUDE_LIST_BASE(std::vector<Ast_Node*> children) : Ast_Node_INCLUDE_LIST(children) {}

    virtual ~Ast_Node_Construction_INCLUDE_LIST_BASE();

    static Ast_Node_Construction_INCLUDE_LIST_BASE* make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


struct Ast_Node_INCLUDE_PATHS_LIST : public Ast_Node {
    Ast_Node_INCLUDE_PATHS_LIST(std::vector<Ast_Node*> children) : Ast_Node(children) {}
    virtual ~Ast_Node_INCLUDE_PATHS_LIST();
};
struct Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE : public Ast_Node_INCLUDE_PATHS_LIST {
    Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE(std::vector<Ast_Node*> children) : Ast_Node_INCLUDE_PATHS_LIST(children) {}

    virtual ~Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE();

    static Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE* make(Ast_Node* p0);

    virtual const char* get_name() override;
    virtual void traverse() override;
};

struct Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE : public Ast_Node_INCLUDE_PATHS_LIST {
    Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE(std::vector<Ast_Node*> children) : Ast_Node_INCLUDE_PATHS_LIST(children) {}

    virtual ~Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE();

    static Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE* make(Ast_Node* p0, Ast_Node* p1);

    virtual const char* get_name() override;
    virtual void traverse() override;
};


