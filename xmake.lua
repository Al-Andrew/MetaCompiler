add_rules("mode.debug", "mode.release")
add_requires("nlohmann_json")

set_warnings("all")
add_languages("c++20")
set_version("0.1.0")

target("meta_compiler")
    set_filename("mc")
    set_kind("binary")
    add_files("src/main.cpp")

    add_packages("nlohmann_json")

    after_build(function (target)
        os.mkdir("bin")
        os.cp(target:targetfile(), "bin/")
    end)