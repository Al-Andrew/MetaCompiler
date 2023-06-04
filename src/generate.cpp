#include "mc/generate.hpp"

#include "mc/logging.hpp"
#include "mc/stencil.hpp"
#include "mc/utils.hpp"

#include <cstdlib>
#include <filesystem>

namespace mc {

static constexpr std::string_view cmake_project_stencil = R"__cmake(
cmake_minimum_required(VERSION 3.22)

project(/* @project_name */ LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
)__cmake";

static constexpr std::string_view cmake_src_stencil = R"__cmake(
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
# Set the input files
set(PARSER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/parser.y)
set(LEXER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l)

# Set the output files
set(PARSER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
set(LEXER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

# Generate the parser and lexer sources
BISON_TARGET(Parser ${PARSER_INPUT} ${PARSER_OUTPUT} COMPILE_FLAGS "-d")
FLEX_TARGET(Lexer ${LEXER_INPUT} ${LEXER_OUTPUT} COMPILE_FLAGS "--header=lexer.hpp")

# Include the generated sources in the build
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    Ast.cpp
    Rules.cpp
)

add_executable(/* @bin */ ${SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
)__cmake";

void
generate_makefile(Language_Description ld, std::ofstream &proj, std::ofstream &src) noexcept {
    MC_TRACE_FUNCTION("");
    using mc::Stencil;

    Stencil proj_stencil(cmake_project_stencil, proj);

    auto current_ident = proj_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@project_name\" identifier in CMakeLists.txt stencil");
    proj_stencil.file << ld.name;
    current_ident = proj_stencil.push_untill_identifier();
    MC_CHECK_EXIT(!current_ident.has_value(), "expected no more identifiers in CMakeLists.txt stencil");

    Stencil src_stencil(cmake_src_stencil, src);

    current_ident = src_stencil.push_untill_identifier();
    MC_CHECK_EXIT(current_ident.has_value(), "expected \"@bin_name\" identifier in src/CMakeLists.txt stencil");
    src_stencil.file << ld.bin_name;
    current_ident = src_stencil.push_untill_identifier();
}

void
generate_lex_lexer(Language_Description ld, std::ofstream file) noexcept;

void
generate_yacc_parser(Language_Description ld, std::ofstream file) noexcept;

void
generate_ast(Language_Description ld, std::ofstream header, std::ofstream file) noexcept;

void
generate_rules(Language_Description ld, std::ofstream header, std::ofstream file) noexcept;

void
generate_executable(std::filesystem::path output_dir) noexcept {
    MC_TRACE_FUNCTION("");
    std::filesystem::path original_path = std::filesystem::current_path();
    MC_DEBUG("cwd \"{}\"", std::filesystem::current_path().string());
    std::filesystem::current_path(output_dir);
    MC_DEBUG("cwd \"{}\"", std::filesystem::current_path().string());

    std::filesystem::create_directory("build");
    std::system("cmake -B build/ -S .");
    MC_DEBUG("cwd \"{}\"", std::filesystem::current_path().string());
    std::system("cmake --build build/");

    std::filesystem::current_path(original_path);
    MC_DEBUG("cwd \"{}\"", std::filesystem::current_path().string());
}

void
generate(Language_Description ld, std::filesystem::path output_dir) noexcept {
    MC_TRACE_FUNCTION("");

    MC_CHECK_WARN(
        std::filesystem::create_directory(output_dir),
        "Failed to create output directory \"{}\". If it already exists it will be overriden.", output_dir.string());
    MC_CHECK_WARN(
        std::filesystem::create_directory(output_dir / "src"),
        "Failed to create output directory \"{}/src\". If it already exists it will be overriden.",
        output_dir.string());

    {  // NOTE: generate we need the scope to close the files
        std::ofstream proj_makefile(output_dir / "CMakeLists.txt");
        std::ofstream src_makefile(output_dir / "src" / "CMakeLists.txt");
        generate_makefile(ld, proj_makefile, src_makefile);
    }

    generate_executable(output_dir);
}

}  // namespace mc