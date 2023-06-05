#pragma once
#include "logging.hpp"

#define MC_TRACE_FUNCTION(fmt, ...) MC_TRACE("[{}] " fmt, __FUNCTION__, ##__VA_ARGS__)

#define MC_TRY_OR_EXIT(expr)            \
    try {                               \
        expr;                           \
    } catch (const std::exception &e) { \
        MC_CRITICAL("{}", e.what());    \
        exit(EXIT_FAILURE);             \
    }

#define MC_CHECK_WARN(expr, msg, ...) \
    if (!(expr)) {                    \
        MC_WARN(msg, ##__VA_ARGS__);  \
    }

#define MC_CHECK_EXIT(expr, msg, ...)    \
    if (!(expr)) {                       \
        MC_CRITICAL(msg, ##__VA_ARGS__); \
        exit(EXIT_FAILURE);              \
    }
