// this project:
#include "mc/generate.hpp"
#include "mc/language_description.hpp"
#include "mc/logging.hpp"
#include "mc/options.hpp"
#include "mc/utils.hpp"

int
main(int argc, char **argv) {

    mc::Cmd_Options options = mc::parse_cmd_options(argc, argv);
    mc::logging::init_logging(options.log_level);

    MC_TRACE_FUNCTION(
        "options: input_file: {}, output_dir: {}, log_level: {}", options.input_file.string(),
        options.output_dir.string(), mc::logging::log_level_to_string(options.log_level)->data());

    auto ld = mc::Language_Description::new_from_json(options.input_file);
    ld.validate_rules();

    mc::generate(ld, options.output_dir);
}