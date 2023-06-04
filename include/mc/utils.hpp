#pragma once
#include "logging.hpp"

#define MC_TRACE_FUNCTION(fmt, ...) MC_TRACE("[{}] " fmt, __FUNCTION__, ##__VA_ARGS__)

#define TRY_OR_EXIT(expr)               \
    try {                               \
        expr;                           \
    } catch (const std::exception &e) { \
        MC_CRITICAL("{}", e.what());    \
        exit(EXIT_FAILURE);             \
    }