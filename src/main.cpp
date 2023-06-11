// this project:
#include "mc/generate.hpp"
#include "mc/language_description.hpp"
#include "mc/logging.hpp"
#include "mc/options.hpp"
#include "mc/utils.hpp"
//
//
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"

extern Ast_Node     *ast_root;
extern std::ofstream out_stream;
extern FILE         *yyin;
extern int
yyparse();

int
main(int argc, char **argv) {

    mc::Cmd_Options options = mc::parse_cmd_options(argc, argv);
    mc::logging::init_logging(options.log_level);

    MC_TRACE_FUNCTION(
        "options: input_file: {}, output_dir: {}, log_level: {}", options.input_file.string(),
        options.output_dir.string(), mc::logging::log_level_to_string(options.log_level)->data());

    std::istream *input;
    if (options.is_input_raw) {
        input = new std::ifstream(options.input_file);
    } else {
        MC_INFO("Feeding input file through neodymium");

        MC_CHECK_WARN(
            std::filesystem::create_directory(options.output_dir),
            "Failed to create output directory \"{}\". If it already exists it will be overriden.",
            options.output_dir.string());
        MC_CHECK_WARN(
            std::filesystem::create_directory(options.output_dir / "build"),
            "Failed to create output directory \"{}/src\". If it already exists it will be overriden.",
            options.output_dir.string());
        yyin = fopen(options.input_file.string().c_str(), "r");
        yyparse();
        fclose(yyin);
        out_stream.open(options.output_dir / "build" / "language.json");
        ast_root->traverse();
        out_stream.close();
        input = new std::ifstream(options.output_dir / "build" / "language.json");
    }

    auto ld = mc::Language_Description::new_from_json(*input);
    ld.validate_rules();

    mc::generate(ld, options.output_dir);
}