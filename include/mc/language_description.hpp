#pragma once
#include <filesystem>
#include <istream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace mc {

struct Token {
    std::string name;
    std::string match;
    bool        is_regex;

    Token(std::string name, std::string match, bool is_regex)
        : name(std::move(name)), match(std::move(match)), is_regex(is_regex) {
    }

    [[nodiscard]] std::string
    full_enum_name() const noexcept;
    [[nodiscard]] std::string
    enum_name() const noexcept;
    [[nodiscard]] std::string
    matcher_text() const noexcept;
};

struct Construction;
struct Rule {
    std::string               name;
    std::vector<Construction> constructions;

    Rule(std::string name, std::vector<Construction> constructions)
        : name(std::move(name)), constructions(std::move(constructions)) {
    }

    [[nodiscard]] std::string
    ast_node_name() const noexcept;
};

struct Construction {
    std::string                            tag;
    std::vector<std::string>               symbols;
    std::string                            action;
    std::vector<std::variant<Token, Rule>> symbols_variant;

    Construction(std::string tag, std::vector<std::string> symbols, std::string action)
        : tag(std::move(tag)), symbols(std::move(symbols)), action(std::move(action)) {
    }

    [[nodiscard]] std::string
    ast_node_name(const std::string_view rule_name) const noexcept;
    [[nodiscard]] std::string
    format_action() const noexcept;
};

struct Language_Description {
    std::string                name;
    std::string                bin_name;
    std::vector<Token>         tokens;
    std::vector<Rule>          rules;
    std::string                start_rule;
    std::optional<std::string> main;

    [[nodiscard]] static Language_Description
    new_from_json(std::istream &path) noexcept;

    void
    validate_rules() noexcept;
};

}  // namespace mc
