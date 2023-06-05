#pragma once
#include <filesystem>
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
};

struct Construction {
    std::string                            tag;
    std::vector<std::string>               symbols;
    std::string                            action;
    std::vector<std::variant<Token, Rule>> symbols_variant;

    Construction(std::string tag, std::vector<std::string> symbols, std::string action)
        : tag(std::move(tag)), symbols(std::move(symbols)), action(std::move(action)) {
    }
};

struct Language_Description {
    std::string        name;
    std::string        bin_name;
    std::vector<Token> tokens;
    std::vector<Rule>  rules;
    std::string        start_rule;

    [[nodiscard]] static Language_Description
    new_from_json(std::filesystem::path path) noexcept;

    void
    validate_rules() noexcept;
};

}  // namespace mc