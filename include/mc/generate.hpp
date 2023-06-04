#pragma once
#include "mc/language_description.hpp"

#include <fstream>

namespace mc {

void
generate_makefile(Language_Description ld, std::ofstream &proj, std::ofstream &src) noexcept;

void
generate_lex_lexer(Language_Description ld, std::ofstream file) noexcept;

void
generate_yacc_parser(Language_Description ld, std::ofstream file) noexcept;

void
generate_ast(Language_Description ld, std::ofstream header, std::ofstream file) noexcept;

void
generate_executable(std::filesystem::path output_dir) noexcept;

void
generate(Language_Description ld, std::filesystem::path output_dir) noexcept;

}  // namespace mc