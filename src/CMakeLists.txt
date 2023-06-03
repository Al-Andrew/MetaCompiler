set(SOURCES
    main.cpp
)

add_executable(mc ${SOURCES})
target_include_directories(mc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(mc PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog cxxopts)
