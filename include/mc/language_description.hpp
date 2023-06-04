#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace mc {

struct Token {
    std::string name;
    std::string match;
    bool        is_regex;

    Token(std::string name, std::string match, bool is_regex)
        : name(std::move(name)), match(std::move(match)), is_regex(is_regex) {
    }
};

struct Construction {
    std::string              tag;
    std::vector<std::string> symbols;
    std::string              action;

    Construction(std::string tag, std::vector<std::string> symbols, std::string action)
        : tag(std::move(tag)), symbols(std::move(symbols)), action(std::move(action)) {
    }
};

struct Rule {
    std::string               name;
    std::vector<Construction> constructions;

    Rule(std::string name, std::vector<Construction> constructions)
        : name(std::move(name)), constructions(std::move(constructions)) {
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
    validate_rules() const noexcept;
};

}  // namespace mc