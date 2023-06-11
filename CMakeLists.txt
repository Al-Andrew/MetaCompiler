cmake_minimum_required(VERSION 3.22)

project(MetaComplier)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(thirdparty/nlohmann)

add_subdirectory(thirdparty/spdlog)

set(CXXOPTS_BUILD_TESTS OFF CACHE INTERNAL "")
add_subdirectory(thirdparty/cxxopts)

add_subdirectory(src)
add_subdirectory(neodymium)