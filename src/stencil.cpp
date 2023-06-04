#include "mc/stencil.hpp"

#include "mc/logging.hpp"
#include "mc/utils.hpp"

#include <regex>

namespace mc {

// NOTE: an identifier loks something like this: /* @identifier */
const std::regex Stencil::indentifier_regex = std::regex("/\\*[ ]* @[a-z_]* [ ]*\\*/");

[[nodiscard]] std::optional<std::string>
Stencil::push_untill_identifier() noexcept {
    MC_TRACE_FUNCTION("");

    // FIXME: stupid copy
    std::string haystack(stencil.begin() + current_offset, stencil.end());
    std::smatch match;
    if (std::regex_search(haystack, match, indentifier_regex)) {
        std::string match_str = match.str();
        file << match.prefix();
        current_offset += match.prefix().length() + match_str.length();

        // getting just the identifier part of the match. the find should never fail
        auto start = match_str.find("@") + 1;
        auto end   = match_str.find(" ", start);
        match_str  = match_str.substr(start, end - start);

        return match_str;
    }

    // if we get here we don't have an identifier, so we just push the rest of the stencil
    file << haystack;

    return std::nullopt;
}

}  // namespace mc