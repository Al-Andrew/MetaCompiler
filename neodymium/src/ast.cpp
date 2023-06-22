
#include "ast.hpp"
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

std::unordered_map<std::string, std::string> user_data;
std::vector<std::string> includes;
extern std::ofstream out_stream;
extern void push(int i);
extern int pop();
extern int stack[1024];
extern int stack_pointer;
extern Ast_Node* ast_root;
extern FILE* yyin;
extern int yyparse();

static std::string escape_cpp_literal(const std::string& literal) {
    std::string ret;

    for(const char c : literal) {
        switch(c) {
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

#include <algorithm>

std::vector<std::string> split_params(const std::string& str) {
    std::vector<std::string> constructs;
    std::string buffer;

    bool inBackticks = false;

    for (char ch : str) {
        if (ch == '`') {
            inBackticks = !inBackticks;
        }
        if (ch == ',' && !inBackticks) {
            // Remove leading/trailing spaces
            buffer.erase(buffer.begin(),
                         std::find_if(buffer.begin(), buffer.end(), [](int ch) {
                             return !std::isspace(ch);
                         }));
            buffer.erase(std::find_if(buffer.rbegin(), buffer.rend(),
                                      [](int ch) { return !std::isspace(ch); })
                             .base(),
                         buffer.end());
            buffer = buffer.substr(1, buffer.size() - 2);
            constructs.push_back(buffer);
            buffer.clear();
        } else {
            buffer += ch;
        }
    }
    // Push the last construct
    // Remove leading/trailing spaces
    buffer.erase(buffer.begin(),
                 std::find_if(buffer.begin(), buffer.end(),
                              [](int ch) { return !std::isspace(ch); }));
    buffer.erase(std::find_if(buffer.rbegin(), buffer.rend(),
                              [](int ch) { return !std::isspace(ch); })
                     .base(),
                 buffer.end());
    if(buffer[0] == '`') {
        buffer = buffer.substr(1, buffer.size() - 2);
    }
    printf("is_last_construct: %s\n", buffer.c_str());
    constructs.push_back(buffer);

    return constructs;
}

static std::string expand_component_action(const std::string& code, const std::string& paramList) {
    // the paramList is of the form: p1,p2,p3,p4
    // first we need to split it into a vector of strings
    printf("before split paramList: %s\n", paramList.c_str());
    std::vector<std::string> params = split_params(paramList);
    

    // now we need to replace the $0, $1, $2 etc. with the params
    std::string ret = "";
    
    for(int idx = 0; idx < code.size() ; ++idx) {
        if(code[idx] == '$') {
            if(code[idx + 1] == '$') {
                ret += '$';
                ++idx;
            } else {
                std::string param_idx_str = "";
                while(idx + 1 < code.size() && code[idx + 1] >= '0' && code[idx + 1] <= '9') {
                    param_idx_str += code[idx + 1];
                    ++idx;
                }
                int param_idx = std::stoi(param_idx_str);
                ret += params[param_idx];
                printf("replaced param %d with %s\n", param_idx, params[param_idx].c_str());
            }
        } else {
            ret += code[idx];
        }
    }

    return ret;
}

Ast_Node::~Ast_Node(){
    for (auto child : children) {
        delete child;
    }
}

Ast_Node_FILE::~Ast_Node_FILE() {}
Ast_Node_Construction_FILE_LANG::~Ast_Node_Construction_FILE_LANG() {}

    Ast_Node_Construction_FILE_LANG* Ast_Node_Construction_FILE_LANG::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4, Ast_Node* p5, Ast_Node* p6) {
        return new Ast_Node_Construction_FILE_LANG{std::vector<Ast_Node*>{p0, p1, p2, p3, p4, p5, p6}};
    }
    const char* Ast_Node_Construction_FILE_LANG::get_name() {
        return "Ast_Node_Construction_FILE_LANG";
    }
    void Ast_Node_Construction_FILE_LANG::traverse() {

                out_stream << "\{\n    \"meta\": {\n        \"name\": " << dynamic_cast<Ast_Node_Token*>(children[2])->token.value
                    << ",\n    \"bin\": \"out.bin\",\n    \"version\": \"0.1.0\"\n   },\n";
                children[3]->traverse();
                children[4]->traverse();
                auto sr = user_data["start_rule"];
                out_stream << "    \"start_rule\": " << sr << ",\n";
                children[5]->traverse();
                out_stream << "\n}\n";
            
    }
Ast_Node_Construction_FILE_LANG_WITH_INCLUDES::~Ast_Node_Construction_FILE_LANG_WITH_INCLUDES() {}

    Ast_Node_Construction_FILE_LANG_WITH_INCLUDES* Ast_Node_Construction_FILE_LANG_WITH_INCLUDES::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4, Ast_Node* p5, Ast_Node* p6, Ast_Node* p7) {
        return new Ast_Node_Construction_FILE_LANG_WITH_INCLUDES{std::vector<Ast_Node*>{p0, p1, p2, p3, p4, p5, p6, p7}};
    }
    const char* Ast_Node_Construction_FILE_LANG_WITH_INCLUDES::get_name() {
        return "Ast_Node_Construction_FILE_LANG_WITH_INCLUDES";
    }
    void Ast_Node_Construction_FILE_LANG_WITH_INCLUDES::traverse() {

                children[3]->traverse();
                printf("done traversing includes\n");
                out_stream << "\{\n    \"meta\": {\n        \"name\": " << dynamic_cast<Ast_Node_Token*>(children[2])->token.value
                    << ",\n    \"bin\": \"out.bin\",\n    \"version\": \"0.1.0\"\n   },\n";
                children[4]->traverse();
                children[5]->traverse();
                auto sr = user_data["start_rule"];
                out_stream << "    \"start_rule\": " << sr << ",\n";
                children[6]->traverse();
                out_stream << "\n}\n";
            
    }
Ast_Node_Construction_FILE_COMPONENT::~Ast_Node_Construction_FILE_COMPONENT() {}

    Ast_Node_Construction_FILE_COMPONENT* Ast_Node_Construction_FILE_COMPONENT::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4) {
        return new Ast_Node_Construction_FILE_COMPONENT{std::vector<Ast_Node*>{p0, p1, p2, p3, p4}};
    }
    const char* Ast_Node_Construction_FILE_COMPONENT::get_name() {
        return "Ast_Node_Construction_FILE_COMPONENT";
    }
    void Ast_Node_Construction_FILE_COMPONENT::traverse() {

                std::string name = dynamic_cast<Ast_Node_Token*>(children[2])->token.value;
                name = name.substr(1, name.size() - 2);
                user_data["current_component"] = name;
                children[3]->traverse();
            
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
Ast_Node_CA_PARAM::~Ast_Node_CA_PARAM() {}
Ast_Node_Construction_CA_PARAM_CA::~Ast_Node_Construction_CA_PARAM_CA() {}

    Ast_Node_Construction_CA_PARAM_CA* Ast_Node_Construction_CA_PARAM_CA::make(Ast_Node* p0) {
        return new Ast_Node_Construction_CA_PARAM_CA{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_CA_PARAM_CA::get_name() {
        return "Ast_Node_Construction_CA_PARAM_CA";
    }
    void Ast_Node_Construction_CA_PARAM_CA::traverse() {

                std::string param = dynamic_cast<Ast_Node_Token*>(children[0])->token.value;
                printf("ca param ca: %s\n", param.c_str());
                user_data["last_component_action_param_list"] += dynamic_cast<Ast_Node_Token*>(children[0])->token.value;
            
    }
Ast_Node_Construction_CA_PARAM_CPP::~Ast_Node_Construction_CA_PARAM_CPP() {}

    Ast_Node_Construction_CA_PARAM_CPP* Ast_Node_Construction_CA_PARAM_CPP::make(Ast_Node* p0) {
        return new Ast_Node_Construction_CA_PARAM_CPP{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_CA_PARAM_CPP::get_name() {
        return "Ast_Node_Construction_CA_PARAM_CPP";
    }
    void Ast_Node_Construction_CA_PARAM_CPP::traverse() {

                std::string code = dynamic_cast<Ast_Node_Token*>(children[0])->token.value;
                user_data["last_component_action_param_list"] += code;
            
    }
Ast_Node_COMPONENT_ACTION_PARAM_LIST::~Ast_Node_COMPONENT_ACTION_PARAM_LIST() {}
Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA::~Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA() {}

    Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA* Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA::make(Ast_Node* p0) {
        return new Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA::get_name() {
        return "Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA";
    }
    void Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_BASE_CA::traverse() {

                children[0]->traverse();
            
    }
Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE::~Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE() {}

    Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE* Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE::make(Ast_Node* p0, Ast_Node* p1) {
        return new Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE{std::vector<Ast_Node*>{p0, p1}};
    }
    const char* Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE::get_name() {
        return "Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE";
    }
    void Ast_Node_Construction_COMPONENT_ACTION_PARAM_LIST_RECURSIVE::traverse() {

                children[0]->traverse();
                if(children[1]->children.size() > 0) 
                    user_data["last_component_action_param_list"] += ",";
                children[1]->traverse();
            
    }
Ast_Node_ACTION_EXPAND::~Ast_Node_ACTION_EXPAND() {}
Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL::~Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL() {}

    Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL* Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL::make(Ast_Node* p0) {
        return new Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL::get_name() {
        return "Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL";
    }
    void Ast_Node_Construction_ACTION_EXPAND_CPP_LITERAL::traverse() {

                std::string code = dynamic_cast<Ast_Node_Token*>(children[0])->token.value;
                code = code.substr(1, code.size() - 2);
                out_stream << escape_cpp_literal(code);
            
    }
Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS::~Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS() {}

    Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS* Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS::get_name() {
        return "Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS";
    }
    void Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_W_PARAMS::traverse() {

                user_data["last_component_action_param_list"] = "";
                children[2]->traverse();
                std::string action_name = dynamic_cast<Ast_Node_Token*>(children[1])->token.value;
                std::string action_code = user_data[action_name];
                std::string param_list = user_data["last_component_action_param_list"];
                std::string expanded_action = expand_component_action(action_code, param_list);
                out_stream << escape_cpp_literal(expanded_action);
            
    }
Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS::~Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS() {}

    Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS* Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2) {
        return new Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS{std::vector<Ast_Node*>{p0, p1, p2}};
    }
    const char* Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS::get_name() {
        return "Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS";
    }
    void Ast_Node_Construction_ACTION_EXPAND_COMPONENT_ACTION_WO_PARAMS::traverse() {

                user_data["last_component_action_param_list"] = "";
                std::string action_name = dynamic_cast<Ast_Node_Token*>(children[1])->token.value;
                std::string action_code = user_data[action_name];
                std::string param_list = user_data["last_component_action_param_list"];
                std::string expanded_action = expand_component_action(action_code, param_list);
                out_stream << escape_cpp_literal(expanded_action);
            
    }
Ast_Node_ACTION_EXPAND_LIST::~Ast_Node_ACTION_EXPAND_LIST() {}
Ast_Node_Construction_ACTION_EXPAND_LIST_BASE::~Ast_Node_Construction_ACTION_EXPAND_LIST_BASE() {}

    Ast_Node_Construction_ACTION_EXPAND_LIST_BASE* Ast_Node_Construction_ACTION_EXPAND_LIST_BASE::make(Ast_Node* p0) {
        return new Ast_Node_Construction_ACTION_EXPAND_LIST_BASE{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_ACTION_EXPAND_LIST_BASE::get_name() {
        return "Ast_Node_Construction_ACTION_EXPAND_LIST_BASE";
    }
    void Ast_Node_Construction_ACTION_EXPAND_LIST_BASE::traverse() {

                children[0]->traverse();
            
    }
Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE::~Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE() {}

    Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE* Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE::make(Ast_Node* p0, Ast_Node* p1) {
        return new Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE{std::vector<Ast_Node*>{p0, p1}};
    }
    const char* Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE::get_name() {
        return "Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE";
    }
    void Ast_Node_Construction_ACTION_EXPAND_LIST_RECURSIVE::traverse() {

                children[0]->traverse();
                children[1]->traverse();
            
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

                out_stream << "    \"action\": \"";
                children[2]->traverse();
                out_stream << "\"\n";
            
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
Ast_Node_Construction_RULE_WITH_START::~Ast_Node_Construction_RULE_WITH_START() {}

    Ast_Node_Construction_RULE_WITH_START* Ast_Node_Construction_RULE_WITH_START::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4, Ast_Node* p5, Ast_Node* p6) {
        return new Ast_Node_Construction_RULE_WITH_START{std::vector<Ast_Node*>{p0, p1, p2, p3, p4, p5, p6}};
    }
    const char* Ast_Node_Construction_RULE_WITH_START::get_name() {
        return "Ast_Node_Construction_RULE_WITH_START";
    }
    void Ast_Node_Construction_RULE_WITH_START::traverse() {

                user_data["start_rule"] = dynamic_cast<Ast_Node_Token*>(children[1])->token.value;
                out_stream << "    {\n";
                out_stream << "        \"name\": " << dynamic_cast<Ast_Node_Token*>(children[1])->token.value << ",\n";
                out_stream << "        \"constructions\": [\n";
                children[5]->traverse();
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

                out_stream << "    \"main\": \"";
                children[2]->traverse();
                out_stream << "\"\n";
            
    }
Ast_Node_COMPONENT_ACTION::~Ast_Node_COMPONENT_ACTION() {}
Ast_Node_Construction_COMPONENT_ACTION_BASE::~Ast_Node_Construction_COMPONENT_ACTION_BASE() {}

    Ast_Node_Construction_COMPONENT_ACTION_BASE* Ast_Node_Construction_COMPONENT_ACTION_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3, Ast_Node* p4) {
        return new Ast_Node_Construction_COMPONENT_ACTION_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3, p4}};
    }
    const char* Ast_Node_Construction_COMPONENT_ACTION_BASE::get_name() {
        return "Ast_Node_Construction_COMPONENT_ACTION_BASE";
    }
    void Ast_Node_Construction_COMPONENT_ACTION_BASE::traverse() {

                std::string action_name = dynamic_cast<Ast_Node_Token*>(children[2])->token.value;
                action_name = action_name.substr(1, action_name.size() - 2);
                std::string action_code = dynamic_cast<Ast_Node_Token*>(children[3])->token.value;
                action_code = action_code.substr(1, action_code.size() - 2);
                std::string action_full_name = user_data["current_component"] + "." + action_name;
                printf("action: %s\n", action_full_name.c_str());
                user_data[action_full_name] = action_code;
            
    }
Ast_Node_COMPONENT_ACTION_LIST::~Ast_Node_COMPONENT_ACTION_LIST() {}
Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE::~Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE() {}

    Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE* Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE::make(Ast_Node* p0) {
        return new Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE::get_name() {
        return "Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE";
    }
    void Ast_Node_Construction_COMPONENT_ACTION_LIST_BASE::traverse() {

                children[0]->traverse();
            
    }
Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE::~Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE() {}

    Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE* Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE::make(Ast_Node* p0, Ast_Node* p1) {
        return new Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE{std::vector<Ast_Node*>{p0, p1}};
    }
    const char* Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE::get_name() {
        return "Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE";
    }
    void Ast_Node_Construction_COMPONENT_ACTION_LIST_RECURSIVE::traverse() {

                children[0]->traverse();
                children[1]->traverse();
            
    }
Ast_Node_INCLUDE_LIST::~Ast_Node_INCLUDE_LIST() {}
Ast_Node_Construction_INCLUDE_LIST_BASE::~Ast_Node_Construction_INCLUDE_LIST_BASE() {}

    Ast_Node_Construction_INCLUDE_LIST_BASE* Ast_Node_Construction_INCLUDE_LIST_BASE::make(Ast_Node* p0, Ast_Node* p1, Ast_Node* p2, Ast_Node* p3) {
        return new Ast_Node_Construction_INCLUDE_LIST_BASE{std::vector<Ast_Node*>{p0, p1, p2, p3}};
    }
    const char* Ast_Node_Construction_INCLUDE_LIST_BASE::get_name() {
        return "Ast_Node_Construction_INCLUDE_LIST_BASE";
    }
    void Ast_Node_Construction_INCLUDE_LIST_BASE::traverse() {

                children[2]->traverse();
            
    }
Ast_Node_INCLUDE_PATHS_LIST::~Ast_Node_INCLUDE_PATHS_LIST() {}
Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE::~Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE() {}

    Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE* Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE::make(Ast_Node* p0) {
        return new Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE{std::vector<Ast_Node*>{p0}};
    }
    const char* Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE::get_name() {
        return "Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE";
    }
    void Ast_Node_Construction_INCLUDE_PATHS_LIST_BASE::traverse() {

                std::string include = dynamic_cast<Ast_Node_Token*>(children[0])->token.value;
                include = include.substr(1, include.size() - 2);
                Ast_Node* current_root = ast_root;
                Ast_Node* include_root = nullptr;
                printf("Including %s\n", include.c_str());
                yyin=fopen(include.c_str(),"r");
                int result = yyparse();
                fclose(yyin);
                if(result != 0)
                    return;
                include_root = ast_root;
                ast_root = current_root;
                include_root->print();
                printf("Traversing\n");
                include_root->traverse();
                printf("Done traversing\n");
            
    }
Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE::~Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE() {}

    Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE* Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE::make(Ast_Node* p0, Ast_Node* p1) {
        return new Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE{std::vector<Ast_Node*>{p0, p1}};
    }
    const char* Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE::get_name() {
        return "Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE";
    }
    void Ast_Node_Construction_INCLUDE_PATHS_LIST_RECURSIVE::traverse() {

                std::string include = dynamic_cast<Ast_Node_Token*>(children[0])->token.value;
                include = include.substr(1, include.size() - 2);
                Ast_Node* current_root = ast_root;
                Ast_Node* include_root = nullptr;
                printf("Including %s\n", include.c_str());
                yyin=fopen(include.c_str(),"r");
                int result = yyparse();
                fclose(yyin);
                if(result != 0)
                    return;
                include_root = ast_root;
                ast_root = current_root;
                include_root->print();
                printf("Traversing\n");
                include_root->traverse();
                children[1]->traverse();
            
    }
