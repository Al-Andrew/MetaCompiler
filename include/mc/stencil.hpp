#pragma once
#include <fstream>
#include <optional>
#include <regex>
#include <string_view>

namespace mc {

struct Stencil {
    static const std::regex indentifier_regex;
    size_t                  current_offset = 0;
    std::string_view        stencil;
    std::ofstream          &file;

    Stencil(std::string_view stencil, std::ofstream &file) noexcept : stencil(std::move(stencil)), file(file) {
    }

    [[nodiscard]] std::optional<std::string>
    push_untill_identifier() noexcept;
};

}  // namespace mc
