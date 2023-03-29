#include <cstdint>
#include <variant>
#include <memory>
#include <fstream>
#include <iostream>
#include <functional>
#include <sstream>
#include <vector>
#include <utility>
#include <string_view>

class [[nodiscard]] Generic_Error {
public:
    std::string_view message;
};

template<class Result_T, class Error_T = Generic_Error>
class [[nodiscard]] Error_Or {
public:
    using Result_Type = Result_T;
    using Error_Type = Error_T;

    constexpr static auto make_error(Error_Type& error_value) noexcept -> Error_Or {
        return Error_Or(std::forward(error_value));
    }

    constexpr static auto make_result(Result_Type&& result_value) noexcept -> Error_Or {
        return Error_Or(std::forward(result_value));
    }

    constexpr auto is_error() const noexcept -> bool {
        return m_is_error; 
    }

    constexpr auto is_result() const noexcept -> bool {
        return !m_is_error;
    }

    Error_Or(Error_Or&&) = default;
    auto operator=(Error_Or&&) = default;

    Error_Or(Error_Or const&) = delete;
    auto operator=(Error_Or const&) = delete;

private:
    const bool m_is_error{};
    union {
        Result_Type m_result;
        Error_Type m_error;
    };

    constexpr explicit Error_Or(Error_Type error_value) noexcept
        : m_is_error{true}, m_error{error_value} {}

    constexpr explicit Error_Or(Result_Type result_value) noexcept
        : m_is_error{false}, m_result{result_value} {}
};

class ASTNode;
namespace detail {
    struct Invalid {
    };
    struct Root {
        std::vector<std::shared_ptr<ASTNode>> m_astnodes;
    };
    struct If {
        std::shared_ptr<ASTNode> m_condition;
        std::vector<std::shared_ptr<ASTNode>> m_then_body;
        std::vector<std::shared_ptr<ASTNode>> m_else_body;
    };
    struct Boolean_Expression {
        bool m_value;
    };
    struct BuiltIn_Print {
        std::string_view m_name;
        std::string_view m_fmt;
    };
};

class ASTNode {
public:
    enum Type :std::int32_t {
       INVALID = -1,
       ROOT,
       IF,
       BOOLEAN_EXPRESSION,
       BUILTIN_PRINT,
    } m_type;
    
    
    std::variant<detail::Invalid, detail::Root, detail::If, detail::Boolean_Expression, detail::BuiltIn_Print> m_data;

    template<class T> 
    void set_checked(ASTNode::Type type, T&& data) {
        if (m_type == type) {
            m_data = std::forward<T>(data);
        } else {
            std::cerr << "bad set_checked" << std::endl;
        }
    }

    static auto make_root(std::vector<std::shared_ptr<ASTNode>> nodes) -> std::shared_ptr<ASTNode>{
        ASTNode node;
        node.m_type = Type::ROOT;
        node.set_checked(Type::ROOT, detail::Root{std::move(nodes)});
        return std::make_shared<ASTNode>(node);
    }
    static auto make_if(std::shared_ptr<ASTNode> condition, std::vector<std::shared_ptr<ASTNode>> then_body, std::vector<std::shared_ptr<ASTNode>> else_body) -> std::shared_ptr<ASTNode> {
        ASTNode node;
        node.m_type = Type::IF;
        node.set_checked(Type::IF, detail::If{condition, then_body, else_body});
        return std::make_shared<ASTNode>(node);
    }
    static auto make_boolean_expression(bool value) -> std::shared_ptr<ASTNode> {
        ASTNode node;
        node.m_type = Type::BOOLEAN_EXPRESSION;
        node.set_checked(Type::BOOLEAN_EXPRESSION, detail::Boolean_Expression{value});
        return std::make_shared<ASTNode>(node);
    }
    static auto make_builtin_print(std::string_view name, std::string_view fmt) -> std::shared_ptr<ASTNode> {
        ASTNode node;
        node.m_type = Type::BUILTIN_PRINT;
        node.set_checked(Type::BUILTIN_PRINT, detail::BuiltIn_Print{name, fmt});
        return std::make_shared<ASTNode>(node);
    }
};

class Language_Definition;
using Transpilation_Rule = std::function<void(std::ostream&, Language_Definition, std::shared_ptr<ASTNode>)>;

class Language_Definition {
public:
    std::unordered_map<ASTNode::Type, Transpilation_Rule> m_transpilation_rules;
};

auto apply_transpilation_rule(Language_Definition ld, std::shared_ptr<ASTNode> AST) {
    std::ofstream ss{"transpiled.cpp"};
    if (ld.m_transpilation_rules.find(AST->m_type) != ld.m_transpilation_rules.end()) {
        Transpilation_Rule rule = ld.m_transpilation_rules[AST->m_type];
        rule(ss, ld, AST);
    }
    ss.flush();
}

auto apply_transpilation_rule(Language_Definition ld, std::shared_ptr<ASTNode> AST, std::ostream& ss) -> void {
    if (ld.m_transpilation_rules.find(AST->m_type) != ld.m_transpilation_rules.end()) {
        Transpilation_Rule rule = ld.m_transpilation_rules[AST->m_type];
        rule(ss, ld, AST);
    }
}

int main(int argc, char** argv)
{
    Language_Definition ld;
    ld.m_transpilation_rules.emplace(ASTNode::Type::ROOT, [](std::ostream& ss, Language_Definition ld, std::shared_ptr<ASTNode> AST) {
        static constexpr std::string_view required_headers[] = {"iostream"};
        for (auto& header : required_headers) {
            ss << "#include <" << header << ">" << '\n';
        }
        ss << "int main() {" << '\n';
        for (auto& node : std::get<detail::Root>(AST->m_data).m_astnodes) {
            apply_transpilation_rule(ld, node, ss);
        }
        ss << "}" << std::endl;
    });
    ld.m_transpilation_rules.emplace(ASTNode::Type::IF, [](std::ostream& ss, Language_Definition ld, std::shared_ptr<ASTNode> AST) {
        ss << "if (";
        apply_transpilation_rule(ld, std::get<detail::If>(AST->m_data).m_condition, ss);
        ss << ") {" << '\n';
        for (auto& node : std::get<detail::If>(AST->m_data).m_then_body) {
            apply_transpilation_rule(ld, node, ss);
        }
        ss << "}" << '\n';
        ss << "else {" << '\n';
        for (auto& node : std::get<detail::If>(AST->m_data).m_else_body) {
            apply_transpilation_rule(ld, node, ss);
        }
        ss << "}" << '\n';
    });
    ld.m_transpilation_rules.emplace(ASTNode::Type::BOOLEAN_EXPRESSION, [](std::ostream& ss, Language_Definition ld, std::shared_ptr<ASTNode> AST) {
        ss << ((std::get<detail::Boolean_Expression>(AST->m_data).m_value) ? "true" : "false");
    });
    ld.m_transpilation_rules.emplace(ASTNode::Type::BUILTIN_PRINT, [](std::ostream& ss, Language_Definition ld, std::shared_ptr<ASTNode> AST) {
        ss << "std::cout << " << std::get<detail::BuiltIn_Print>(AST->m_data).m_fmt << " << std::endl;" << '\n';
    });

    auto root = ASTNode::make_root({
        ASTNode::make_if(
            ASTNode::make_boolean_expression(true),
            {
                ASTNode::make_builtin_print("print", "\"hello world\""),
            },
            {
                ASTNode::make_builtin_print("print", "\"goodbye world\""),
            }
        )
    });

    apply_transpilation_rule(ld, root);
    return 0;
}
