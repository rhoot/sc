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

    dofile "sc.lua"
    dofile "sc_tests.lua"

    startproject "sc_tests"