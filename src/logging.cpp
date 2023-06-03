#include "mc/logging.hpp"

#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/stdout_sinks.h"

#include <memory>
#include <optional>

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

auto detail::LoggersContainer::stdout = spdlog::stdout_color_mt("console");
void
init_logging(const spdlog::level::level_enum level) {
    using namespace detail;

    LoggersContainer::stdout->set_level(level);
    if (static_cast<unsigned int>(level) <= static_cast<unsigned int>(spdlog::level::level_enum::debug)) {
        LoggersContainer::stdout->set_pattern("%^ [%R] [%-9l] [%@] %v %$");
    } else {
        LoggersContainer::stdout->set_pattern("%^ [%R] [%-9l] %v ^$");
    }
}

}  // namespace mc::logging