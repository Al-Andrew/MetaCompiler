#pragma once
#include "mc/language_description.hpp"

#include <fstream>

namespace mc {

void
generate(Language_Description ld, std::filesystem::path output_dir) noexcept;

}  // namespace mc