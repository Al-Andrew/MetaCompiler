add_rules("mode.debug", "mode.release")
add_requires("nlohmann_json")

set_warnings("all")
add_languages("c++20")

target("meta_compiler")
    set_filename("mc")
    set_kind("binary")
    add_files("src/main.cpp")

    add_packages("nlohmann_json")