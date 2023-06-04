// mc:
#include "mc/options.hpp"

#include "mc/logging.hpp"

// thirdparty:
#include "cxxopts.hpp"
// std:
#include <iostream>

#define TRY_OR_HELP(expr, msg)                  \
    try {                                       \
        expr;                                   \
    } catch (const std::exception &e) {         \
        std::cerr << "[ERROR] " << msg << '\n'; \
        std::cerr << e.what() << '\n';          \
        std::cerr << cmd.help() << std::endl;   \
        exit(1);                                \
    }

namespace mc {

Cmd_Options
parse_cmd_options(int argc, char **argv) {

    cxxopts::Options cmd("mc", "Framework for generating compilers.");

    cmd.add_options()  //
        ("l,log", "Loging level", cxxopts::value<std::string>()->default_value("WARN"))  //
        ("i,input", "Input file path", cxxopts::value<std::string>())  //
        ("o,output", "Output dir path", cxxopts::value<std::string>()->default_value(".mc/"))  //
        ("h,help", "Print usage");

    cxxopts::ParseResult options;
    TRY_OR_HELP(options = cmd.parse(argc, argv), "Invalid command line arguments");

    if (options.count("help")) {
        std::cout << cmd.help() << std::endl;
        exit(0);
    }

    Cmd_Options ret;
    TRY_OR_HELP(
        ret.log_level = mc::logging::string_to_log_level(options["l"].as<std::string>()).value(), "Invalid log level");
    TRY_OR_HELP(ret.input_file = options["i"].as<std::string>(), "Input file path is required");
    TRY_OR_HELP(ret.output_dir = options["o"].as<std::string>(), "Output dir path is required");

    return ret;
}

}  // namespace mc