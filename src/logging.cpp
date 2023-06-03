#include "mc/logging.hpp"

#include "spdlog/common.h"

namespace mc::logging {
using namespace std::string_view_literals;

static std::array log_level_names = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL", "OFF",
};

std::optional<spdlog::level::level_enum>
string_to_log_level(const std::string_view level) {

    for (size_t i = 0; i < log_level_names.size(); ++i) {
        if (level == log_level_names[i]) {
            return static_cast<spdlog::level::level_enum>(i);
        }
    }

    return std::nullopt;
}

}  // namespace mc::logging