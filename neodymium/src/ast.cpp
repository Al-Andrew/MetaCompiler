
#include "ast.hpp"
#include <fstream>

extern std::ofstream out_stream;

static std::string escape_cpp_literal(const std::string& literal) {
    std::string ret;

    for(const char c : literal) {
        switch(c) {
            case '`': ret += "\""; break;
            case '\n': ret += "\\n"; break;
            case '\t': ret += "\\t"; break;
            case '\r': ret += "\\r"; break;
            case '\v': ret += "\\v"; break;
            case '\b': ret += "\\b"; break;
            case '\f': ret += "\\f"; break;
            case '\a': ret += "\\a"; break;
            case '\\': ret += "\\\\"; break;
            case '\'': ret += "\\'"; break;
            case '\"': ret += "\\\""; break;
            default: ret += c; break;
        }
    }

    return ret;
}

Ast_Node::~Ast_Node(){
    for (auto child : children) {
        delete child;
    }
}

Ast_Node_LANG_DEF::~Ast_Node_LANG_DEF() {}
Ast_Node_Construction_LANG_DEF_BASE::~Ast_Node_Construction_LANG_DEF_BASE() {}

    Ast_Node_Construction_LANG_DEF_BASE* Ast_Node_Construction_LANG_DEF_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4, Ast_Node* p5, Ast_Node* p6, Ast_Node* p7) {
        return new Ast_Node_Construction_LANG_DEF_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3, p4, p5, p6, p7}};
    }
    const char* Ast_Node_Construction_LANG_DEF_BASE::get_name() {
        return "Ast_Node_Construction_LANG_DEF_BASE";
    }
    void Ast_Node_Construction_LANG_DEF_BASE::traverse() {

                out_stream << "\{\n    \"meta\": {\n        \"name\": " << dynamic_cast<Ast_Node_Token*>(children[2])->token.value
                    << ",\n    \"bin\": \"out.bin\",\n    \"version\": \"0.1.0\"\n   },\n";
                children[3]->traverse();
                children[4]->traverse();
                children[5]->traverse();
                children[6]->traverse();
                out_stream << "\n}\n";
            
    }
Ast_Node_START_DEF::~Ast_Node_START_DEF() {}
Ast_Node_Construction_START_DEF_BASE::~Ast_Node_Construction_START_DEF_BASE() {}

    Ast_Node_Construction_START_DEF_BASE* Ast_Node_Construction_START_DEF_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_START_DEF_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_START_DEF_BASE::get_name() {
        return "Ast_Node_Construction_START_DEF_BASE";
    }
    void Ast_Node_Construction_START_DEF_BASE::traverse() {

                out_stream << "    \"start_rule\": " << dynamic_cast<Ast_Node_Token*>(children[2])->token.value << ",\n";
            
    }
Ast_Node_TOKEN::~Ast_Node_TOKEN() {}
Ast_Node_Construction_TOKEN_TEXT::~Ast_Node_Construction_TOKEN_TEXT() {}

    Ast_Node_Construction_TOKEN_TEXT* Ast_Node_Construction_TOKEN_TEXT::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4) {
        return new Ast_Node_Construction_TOKEN_TEXT{std::vector<Ast_Node*>{p0, p1, p2, p3, p4}};
    }
    const char* Ast_Node_Construction_TOKEN_TEXT::get_name() {
        return "Ast_Node_Construction_TOKEN_TEXT";
    }
    void Ast_Node_Construction_TOKEN_TEXT::traverse() {

                out_stream << "        {\n            \"name\":" << dynamic_cast<Ast_Node_Token*>(children[2])->token.value
                 << ",\n            \"is_regex\": false,\n            \"matcher\": "
                 << dynamic_cast<Ast_Node_Token*>(children[3])->token.value << "\n        }";
            
    }
Ast_Node_Construction_TOKEN_REGEX::~Ast_Node_Construction_TOKEN_REGEX() {}

    Ast_Node_Construction_TOKEN_REGEX* Ast_Node_Construction_TOKEN_REGEX::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4) {
        return new Ast_Node_Construction_TOKEN_REGEX{std::vector<Ast_Node*>{p0, p1, p2, p3, p4}};
    }
    const char* Ast_Node_Construction_TOKEN_REGEX::get_name() {
        return "Ast_Node_Construction_TOKEN_REGEX";
    }
    void Ast_Node_Construction_TOKEN_REGEX::traverse() {

                out_stream << "        {\n            \"name\":" << dynamic_cast<Ast_Node_Token*>(children[2])->token.value
                 << ",\n            \"is_regex\": true,\n            \"matcher\": " 
                 << dynamic_cast<Ast_Node_Token*>(children[3])->token.value << "\n        }";
            
    }
Ast_Node_TOKENS_LIST::~Ast_Node_TOKENS_LIST() {}
Ast_Node_Construction_TOKENS_LIST_BASE::~Ast_Node_Construction_TOKENS_LIST_BASE() {}

    Ast_Node_Construction_TOKENS_LIST_BASE* Ast_Node_Construction_TOKENS_LIST_BASE::make(Ast_Node* p0) {
        return new Ast_Node_Construction_TOKENS_LIST_BASE{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_TOKENS_LIST_BASE::get_name() {
        return "Ast_Node_Construction_TOKENS_LIST_BASE";
    }
    void Ast_Node_Construction_TOKENS_LIST_BASE::traverse() {

                children[0]->traverse();
            
    }
Ast_Node_Construction_TOKENS_LIST_RECURSIVE::~Ast_Node_Construction_TOKENS_LIST_RECURSIVE() {}

    Ast_Node_Construction_TOKENS_LIST_RECURSIVE* Ast_Node_Construction_TOKENS_LIST_RECURSIVE::make(Ast_Node* p0, Ast_Node* p1) {
        return new Ast_Node_Construction_TOKENS_LIST_RECURSIVE{std::vector<Ast_Node*>{p0, p1}};
    }
    const char* Ast_Node_Construction_TOKENS_LIST_RECURSIVE::get_name() {
        return "Ast_Node_Construction_TOKENS_LIST_RECURSIVE";
    }
    void Ast_Node_Construction_TOKENS_LIST_RECURSIVE::traverse() {

                children[0]->traverse();
                if(children[1]->children.size() > 0) 
                    out_stream << ",\n";
                children[1]->traverse();
            
    }
Ast_Node_TOKENS_DEF::~Ast_Node_TOKENS_DEF() {}
Ast_Node_Construction_TOKENS_DEF_BASE::~Ast_Node_Construction_TOKENS_DEF_BASE() {}

    Ast_Node_Construction_TOKENS_DEF_BASE* Ast_Node_Construction_TOKENS_DEF_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_TOKENS_DEF_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_TOKENS_DEF_BASE::get_name() {
        return "Ast_Node_Construction_TOKENS_DEF_BASE";
    }
    void Ast_Node_Construction_TOKENS_DEF_BASE::traverse() {

                out_stream << "    \"tokens\": [\n";
                children[2]->traverse();
                out_stream << "\n    ],\n";
            
    }
Ast_Node_ACTION_DEF::~Ast_Node_ACTION_DEF() {}
Ast_Node_Construction_ACTION_DEF_BASE::~Ast_Node_Construction_ACTION_DEF_BASE() {}

    Ast_Node_Construction_ACTION_DEF_BASE* Ast_Node_Construction_ACTION_DEF_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_ACTION_DEF_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_ACTION_DEF_BASE::get_name() {
        return "Ast_Node_Construction_ACTION_DEF_BASE";
    }
    void Ast_Node_Construction_ACTION_DEF_BASE::traverse() {

                out_stream << "    \"action\": " << escape_cpp_literal(dynamic_cast<Ast_Node_Token*>(children[2])->token.value) << "\n";
            
    }
Ast_Node_SYMBOLS_LIST::~Ast_Node_SYMBOLS_LIST() {}
Ast_Node_Construction_SYMBOLS_LIST_BASE::~Ast_Node_Construction_SYMBOLS_LIST_BASE() {}

    Ast_Node_Construction_SYMBOLS_LIST_BASE* Ast_Node_Construction_SYMBOLS_LIST_BASE::make(Ast_Node* p0) {
        return new Ast_Node_Construction_SYMBOLS_LIST_BASE{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_SYMBOLS_LIST_BASE::get_name() {
        return "Ast_Node_Construction_SYMBOLS_LIST_BASE";
    }
    void Ast_Node_Construction_SYMBOLS_LIST_BASE::traverse() {

                out_stream << "        " << dynamic_cast<Ast_Node_Token*>(children[0])->token.value;
            
    }
Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE::~Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE() {}

    Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE* Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE::make(Ast_Node* p0, Ast_Node* p1) {
        return new Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE{std::vector<Ast_Node*>{p0, p1}};
    }
    const char* Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE::get_name() {
        return "Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE";
    }
    void Ast_Node_Construction_SYMBOLS_LIST_RECURSIVE::traverse() {

                out_stream << dynamic_cast<Ast_Node_Token*>(children[0])->token.value;
                if(children[1]->children.size() > 0)
                    out_stream << ",\n";
                children[1]->traverse();
            
    }
Ast_Node_SYMBOLS_DEF::~Ast_Node_SYMBOLS_DEF() {}
Ast_Node_Construction_SYMBOLS_DEF_BASE::~Ast_Node_Construction_SYMBOLS_DEF_BASE() {}

    Ast_Node_Construction_SYMBOLS_DEF_BASE* Ast_Node_Construction_SYMBOLS_DEF_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_SYMBOLS_DEF_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_SYMBOLS_DEF_BASE::get_name() {
        return "Ast_Node_Construction_SYMBOLS_DEF_BASE";
    }
    void Ast_Node_Construction_SYMBOLS_DEF_BASE::traverse() {

                children[2]->traverse();
            
    }
Ast_Node_CONSTRUCTION::~Ast_Node_CONSTRUCTION() {}
Ast_Node_Construction_CONSTRUCTION_BASE::~Ast_Node_Construction_CONSTRUCTION_BASE() {}

    Ast_Node_Construction_CONSTRUCTION_BASE* Ast_Node_Construction_CONSTRUCTION_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4) {
        return new Ast_Node_Construction_CONSTRUCTION_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3, p4}};
    }
    const char* Ast_Node_Construction_CONSTRUCTION_BASE::get_name() {
        return "Ast_Node_Construction_CONSTRUCTION_BASE";
    }
    void Ast_Node_Construction_CONSTRUCTION_BASE::traverse() {

                out_stream << "          {\n";
                out_stream << "               \"tag\": " << dynamic_cast<Ast_Node_Token*>(children[1])->token.value << ",\n";
                out_stream << "               \"symbols\": [\n";
                children[2]->traverse();
                out_stream << "\n               ],\n";
                children[3]->traverse();
                out_stream << "\n          }";
            
    }
Ast_Node_CONSTRUCTIONS_LIST::~Ast_Node_CONSTRUCTIONS_LIST() {}
Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE::~Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE() {}

    Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE* Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE::make(Ast_Node* p0) {
        return new Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE::get_name() {
        return "Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE";
    }
    void Ast_Node_Construction_CONSTRUCTIONS_LIST_BASE::traverse() {

                children[0]->traverse();
            
    }
Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE::~Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE() {}

    Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE* Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE::make(Ast_Node* p0, Ast_Node* p1) {
        return new Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE{std::vector<Ast_Node*>{p0, p1}};
    }
    const char* Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE::get_name() {
        return "Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE";
    }
    void Ast_Node_Construction_CONSTRUCTIONS_LIST_RECURSIVE::traverse() {

                children[0]->traverse();
                if(children[1]->children.size() > 0)
                    out_stream << ",\n";
                children[1]->traverse();
            
    }
Ast_Node_CONSTRUCTIONS_DEF::~Ast_Node_CONSTRUCTIONS_DEF() {}
Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE::~Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE() {}

    Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE* Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE::get_name() {
        return "Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE";
    }
    void Ast_Node_Construction_CONSTRUCTIONS_DEF_BASE::traverse() {

                children[2]->traverse();
            
    }
Ast_Node_RULE::~Ast_Node_RULE() {}
Ast_Node_Construction_RULE_BASE::~Ast_Node_Construction_RULE_BASE() {}

    Ast_Node_Construction_RULE_BASE* Ast_Node_Construction_RULE_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_RULE_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_RULE_BASE::get_name() {
        return "Ast_Node_Construction_RULE_BASE";
    }
    void Ast_Node_Construction_RULE_BASE::traverse() {

                out_stream << "    {\n";
                out_stream << "        \"name\": " << dynamic_cast<Ast_Node_Token*>(children[1])->token.value << ",\n";
                out_stream << "        \"constructions\": [\n";
                children[2]->traverse();
                out_stream << "\n        ]\n";
                out_stream << "    }";
            
    }
Ast_Node_RULES_LIST::~Ast_Node_RULES_LIST() {}
Ast_Node_Construction_RULES_LIST_BASE::~Ast_Node_Construction_RULES_LIST_BASE() {}

    Ast_Node_Construction_RULES_LIST_BASE* Ast_Node_Construction_RULES_LIST_BASE::make(Ast_Node* p0) {
        return new Ast_Node_Construction_RULES_LIST_BASE{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_RULES_LIST_BASE::get_name() {
        return "Ast_Node_Construction_RULES_LIST_BASE";
    }
    void Ast_Node_Construction_RULES_LIST_BASE::traverse() {

                children[0]->traverse();
            
    }
Ast_Node_Construction_RULES_LIST_RECURSIVE::~Ast_Node_Construction_RULES_LIST_RECURSIVE() {}

    Ast_Node_Construction_RULES_LIST_RECURSIVE* Ast_Node_Construction_RULES_LIST_RECURSIVE::make(Ast_Node* p0, Ast_Node* p1) {
        return new Ast_Node_Construction_RULES_LIST_RECURSIVE{std::vector<Ast_Node*>{p0, p1}};
    }
    const char* Ast_Node_Construction_RULES_LIST_RECURSIVE::get_name() {
        return "Ast_Node_Construction_RULES_LIST_RECURSIVE";
    }
    void Ast_Node_Construction_RULES_LIST_RECURSIVE::traverse() {

                children[0]->traverse();
                if(children[1]->children.size() > 0)
                    out_stream << ",\n";
                children[1]->traverse();
            
    }
Ast_Node_RULES_DEF::~Ast_Node_RULES_DEF() {}
Ast_Node_Construction_RULES_DEF_BASE::~Ast_Node_Construction_RULES_DEF_BASE() {}

    Ast_Node_Construction_RULES_DEF_BASE* Ast_Node_Construction_RULES_DEF_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_RULES_DEF_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_RULES_DEF_BASE::get_name() {
        return "Ast_Node_Construction_RULES_DEF_BASE";
    }
    void Ast_Node_Construction_RULES_DEF_BASE::traverse() {

                out_stream << "    \"rules\": [\n";
                children[2]->traverse();
                out_stream << "\n    ],\n";
            
    }
Ast_Node_MAIN_DEF::~Ast_Node_MAIN_DEF() {}
Ast_Node_Construction_MAIN_DEF_BASE::~Ast_Node_Construction_MAIN_DEF_BASE() {}

    Ast_Node_Construction_MAIN_DEF_BASE* Ast_Node_Construction_MAIN_DEF_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_MAIN_DEF_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_MAIN_DEF_BASE::get_name() {
        return "Ast_Node_Construction_MAIN_DEF_BASE";
    }
    void Ast_Node_Construction_MAIN_DEF_BASE::traverse() {

                out_stream << "    \"main\": " << escape_cpp_literal(dynamic_cast<Ast_Node_Token*>(children[2])->token.value) << "\n";
            
    }
