#pragma once
#include "spdlog/spdlog.h"

#include <optional>

namespace mc {
namespace logging {

std::optional<spdlog::level::level_enum>
string_to_log_level(const std::string_view level);

}  // namespace logging
}  // namespace mc