// mc:
#include "mc/language_description.hpp"

#include "mc/utils.hpp"

// thirdparty
#include "nlohmann/json.hpp"

#include <optional>
using json = nlohmann::json;
// std:
#include <fstream>

namespace mc {

[[nodiscard]] std::string
Token::full_enum_name() const noexcept {
    std::string result = "Token_Type::TKN_ID_";

    for (auto c : name) {
        if (std::isalnum(c)) {
            result += std::toupper(c);
        } else {
            result += '_';
        }
    }

    return result;
}

[[nodiscard]] std::string
Token::enum_name() const noexcept {
    std::string result = "TKN_ID_";

    for (auto c : name) {
        if (std::isalnum(c)) {
            result += std::toupper(c);
        } else {
            result += '_';
        }
    }

    return result;
}
[[nodiscard]] std::string
Token::matcher_text() const noexcept {
    if (!is_regex) {
        return '"' + match + '\"';
    } else {
        return match;
    }
}

[[nodiscard]] std::string
Rule::ast_node_name() const noexcept {
    std::string result = "Ast_Node_";

    for (auto c : name) {
        if (std::isalnum(c)) {
            result += std::toupper(c);
        } else {
            result += '_';
        }
    }

    return result;
}

[[nodiscard]] std::string
Construction::ast_node_name(const std::string_view rule_name) const noexcept {
    std::string result = "Ast_Node_Construction_";

    for (auto c : rule_name) {
        if (std::isalnum(c)) {
            result += std::toupper(c);
        } else {
            result += '_';
        }
    }
    result += '_';

    for (auto c : tag) {
        if (std::isalnum(c)) {
            result += std::toupper(c);
        } else {
            result += '_';
        }
    }

    return result;
}

[[nodiscard]] std::string
Construction::format_action() const noexcept {
    std::stringstream sos{};
    std::stringstream sis = std::stringstream(action);
    sis >> std::noskipws;
    while (!sis.eof()) {
        char c{0};
        sis >> c;

        if (c == '\0' || sis.eof()) break;

        if (c == '$') {  // TODO escape
            unsigned int index;
            sis >> index;
            MC_CHECK_EXIT(index < symbols.size(), "index out of bounds in action for construction {}", tag);
            sos << "children[" << index << "]";
        } else {
            sos << c;
        }
    }

    return sos.str();
}

[[nodiscard]] Language_Description
Language_Description::new_from_json(std::filesystem::path path) noexcept {
    MC_TRACE_FUNCTION("");

    Language_Description result;
    json                 json_file;

    MC_TRY_OR_EXIT(json_file = json::parse(std::ifstream(path)));

    json meta;
    MC_TRY_OR_EXIT(meta = json_file["meta"]);
    MC_CHECK_EXIT(meta.is_object(), "expected \"meta\" field to be an object");
    MC_CHECK_EXIT(meta.contains("name"), "expected \"meta\" field to contain \"name\"");
    MC_CHECK_EXIT(meta["name"].is_string(), "expected \"meta\" field to contain a string \"name\"");
    result.name = meta["name"].get<std::string>();
    MC_CHECK_EXIT(meta.contains("bin"), "expected \"meta\" field to contain \"bin\"");
    MC_CHECK_EXIT(meta["bin"].is_string(), "expected \"meta\" field to contain a string \"bin\"");
    result.bin_name = meta["bin"].get<std::string>();

    json tokens;
    MC_TRY_OR_EXIT(tokens = json_file["tokens"]);
    MC_CHECK_EXIT(tokens.is_array(), "expected \"tokens\" field to be an array of objects");
    result.tokens.reserve(tokens.size());
    for (const json &t : tokens) {
        MC_CHECK_EXIT(t.is_object(), "expected \"tokens\" field to be an array of objects");
        MC_CHECK_EXIT(t.contains("name"), "expected token to contain \"name\"");
        MC_CHECK_EXIT(t["name"].is_string(), "expected token to contain a string \"name\"");
        std::string name = t["name"].get<std::string>();
        MC_CHECK_EXIT(t.contains("matcher"), "expected token to contain \"matcher\"");
        MC_CHECK_EXIT(t["matcher"].is_string(), "expected token to contain a string \"matcher\"");
        std::string match    = t["matcher"].get<std::string>();
        bool        is_regex = false;
        if (t.contains("is_regex")) {
            MC_CHECK_EXIT(t["is_regex"].is_boolean(), "expected token : \"is_regex\" to be boolean");
            is_regex = t["is_regex"].get<bool>();
        }
        result.tokens.emplace_back(name, match, is_regex);
    }

    json rules;
    MC_TRY_OR_EXIT(rules = json_file["rules"]);
    MC_CHECK_EXIT(rules.is_array(), "expected \"rules\" field to be an array of objects");
    result.rules.reserve(rules.size());
    for (const json &r : rules) {
        MC_CHECK_EXIT(r.is_object(), "expected \"rules\" field to be an array of objects");
        MC_CHECK_EXIT(r.contains("name"), "expected rule to contain \"name\"");
        MC_CHECK_EXIT(r["name"].is_string(), "expected rule to contain a string \"name\"");
        std::string name = r["name"].get<std::string>();
        MC_CHECK_EXIT(r.contains("constructions"), "expected rule to contain \"constructions\"");
        MC_CHECK_EXIT(r["constructions"].is_array(), "expected rule to contain an array \"constructions\"");
        std::vector<Construction> constructions;
        constructions.reserve(r["constructions"].size());
        for (const json &c : r["constructions"]) {
            MC_CHECK_EXIT(c.is_object(), "expected rule to contain an array of objects \"constructions\"");
            MC_CHECK_EXIT(c.contains("tag"), "expected construction to contain \"tag\"");
            MC_CHECK_EXIT(c["tag"].is_string(), "expected construction to contain a string \"tag\"");
            std::string tag = c["tag"].get<std::string>();
            MC_CHECK_EXIT(c.contains("symbols"), "expected construction to contain \"symbols\"");
            MC_CHECK_EXIT(c["symbols"].is_array(), "expected construction to contain an array \"symbols\"");
            std::vector<std::string> symbols;
            symbols.reserve(c["symbols"].size());
            for (const json &s : c["symbols"]) {
                MC_CHECK_EXIT(s.is_string(), "expected construction to contain an array of strings \"symbols\"");
                symbols.emplace_back(s.get<std::string>());
            }
            MC_CHECK_EXIT(c.contains("action"), "expected construction {} of rule {} to contain \"action\"", tag, name);
            MC_CHECK_EXIT(c["action"].is_string(), "expected construction to contain a string \"action\"");
            std::string action = c["action"].get<std::string>();
            constructions.emplace_back(tag, symbols, action);
        }
        result.rules.emplace_back(name, constructions);
    }

    MC_CHECK_EXIT(json_file.contains("start_rule"), "expected \"start_rule\" field");
    MC_CHECK_EXIT(json_file["start_rule"].is_string(), "expected \"start_rule\" field to be a string");
    result.start_rule = json_file["start_rule"].get<std::string>();

    if (json_file.contains("main")) {
        MC_CHECK_EXIT(json_file["main"].is_string(), "expected \"main\" field to be a string");
        result.main = json_file["main"].get<std::string>();
    } else {
        result.main = std::nullopt;
    }

    return result;
}

void
Language_Description::validate_rules() noexcept {
    MC_TRACE_FUNCTION("");

    // check no duplicate rules
    for (size_t i = 0; i < rules.size(); ++i) {
        for (size_t j = i + 1; j < rules.size(); ++j) {
            MC_CHECK_EXIT(rules[i].name != rules[j].name, "duplicate rule name: {}", rules[i].name);
        }
    }
    // check no duplicate tokens
    for (size_t i = 0; i < tokens.size(); ++i) {
        for (size_t j = i + 1; j < tokens.size(); ++j) {
            MC_CHECK_EXIT(tokens[i].name != tokens[j].name, "duplicate token name: {}", tokens[i].name);
        }
    }
    // check no duplicate construction tags in the same rule
    for (const auto &rule : rules) {
        for (size_t i = 0; i < rule.constructions.size(); ++i) {
            for (size_t j = i + 1; j < rule.constructions.size(); ++j) {
                MC_CHECK_EXIT(
                    rule.constructions[i].tag != rule.constructions[j].tag,
                    "duplicate construction tag: {} in rule: {}", rule.constructions[i].tag, rule.name);
            }
        }
    }
    // check no rule name same as token name
    for (const auto &rule : rules) {
        for (const auto &token : tokens) {
            MC_CHECK_EXIT(rule.name != token.name, "rule name and token name conflict: {}", rule.name);
        }
    }
    // check all construction symbols are either tokens or rules
    for (auto &rule : rules) {
        for (auto &construction : rule.constructions) {
            for (const auto &symbol : construction.symbols) {
                bool found = false;

                for (const auto &token : tokens) {
                    if (token.name == symbol) {
                        found = true;
                        construction.symbols_variant.emplace_back(token);
                        break;
                    }
                }
                for (const auto &rule : rules) {
                    if (rule.name == symbol) {
                        found = true;
                        construction.symbols_variant.emplace_back(rule);
                        break;
                    }
                }
                MC_CHECK_EXIT(found, "symbol {} referenced in {}:{} not found", symbol, rule.name, construction.tag);
            }
        }
    }
}

}  // namespace mc
