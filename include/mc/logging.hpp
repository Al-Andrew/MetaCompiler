#pragma once
#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/spdlog.h"

#include <optional>

namespace mc {
namespace logging {

std::optional<spdlog::level::level_enum>
string_to_log_level(const std::string_view level);

void
init_logging(const spdlog::level::level_enum level);

namespace detail {
struct Loggers_Container {
    static std::shared_ptr<spdlog::logger> stdout;
};

}  // namespace detail

}  // namespace logging
}  // namespace mc

#define MC_TRACE(...)    SPDLOG_LOGGER_TRACE(mc::logging::detail::Loggers_Container::stdout, __VA_ARGS__)
#define MC_DEBUG(...)    SPDLOG_LOGGER_DEBUG(mc::logging::detail::Loggers_Container::stdout, __VA_ARGS__)
#define MC_INFO(...)     SPDLOG_LOGGER_INFO(mc::logging::detail::Loggers_Container::stdout, __VA_ARGS__)
#define MC_WARN(...)     SPDLOG_LOGGER_WARN(mc::logging::detail::Loggers_Container::stdout, __VA_ARGS__)
#define MC_ERROR(...)    SPDLOG_LOGGER_ERROR(mc::logging::detail::Loggers_Container::stdout, __VA_ARGS__)
#define MC_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(mc::logging::detail::Loggers_Container::stdout, __VA_ARGS__)
