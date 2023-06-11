
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


set(SOURCES
    ast.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

add_library(language STATIC ${SOURCES})
# Include the generated sources in the build
target_include_directories(language PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(out.bin main.cpp)
target_link_libraries(out.bin PRIVATE language)


