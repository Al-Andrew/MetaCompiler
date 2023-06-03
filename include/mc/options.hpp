#pragma once

#include "spdlog/spdlog.h"

#include <filesystem>

namespace mc {

struct CmdOptions {
    std::filesystem::path     input_file;
    std::filesystem::path     output_dir;
    spdlog::level::level_enum log_level;
};

CmdOptions
parse_cmd_options(int argc, char **argv);

}  // namespace mc
