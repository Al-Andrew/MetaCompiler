cmake_minimum_required(VERSION 3.22)

project(neodymium)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(thirdparty/nlohmann)

add_executable(mc "src/main.cpp")
target_compile_features(mc PRIVATE cxx_std_20)
target_link_libraries(mc PRIVATE nlohmann_json::nlohmann_json)
