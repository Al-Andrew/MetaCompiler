set(SOURCES
    main.cpp
    options.cpp
    logging.cpp
    language_description.cpp
    stencil.cpp
    generate.cpp
)

add_executable(mc ${SOURCES})
target_include_directories(mc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(mc PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog cxxopts language)
