solution "sc"
    configurations {
        "Debug",
        "Release",
    }

    platforms {
        "x32",
        "x64",
    }

    flags {
        "ExtraWarnings",
        "FatalWarnings",
        "NoPCH",
        "StaticRuntime",
        "Symbols",
    }

    set_location()
    startproject "sc_tests"

    dofile "sc.lua"
    dofile "sc_tests.lua"
