solution "sj"
    configurations {
        "Debug",
        "Release",
    }

    platforms {
        "x32",
        "x64",
    }

    location(path.join("../build", _ACTION))

    flags {
        "ExtraWarnings",
        "FatalWarnings",
        "NoExceptions",
        "NoPCH",
        "StaticRuntime",
        "Symbols",
    }

    includedirs {
        "../include",
    }

    configuration {"x32"}
        libdirs {
            path.join("../lib", _ACTION, "x32")
        }

    configuration {"x64"}
        libdirs {
            path.join("../lib", _ACTION, "x64")
        }

    configuration {"vs*"}
        -- warnings caused by the VC headers when compiling without exceptions
        buildoptions {
            "/wd4530", -- C++ exception handler used, but unwind semantics are not enabled
            "/wd4577", -- 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed.
        }

    configuration {}

    dofile "sj.lua"
    dofile "sj_tests.lua"

    startproject "sj_tests"