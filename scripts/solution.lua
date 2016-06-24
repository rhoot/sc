newoption {
    trigger     = "no-xmm-preserve",
    description = "Disables preservation of non-volatile XMM registers. This should *only* be used if no context will be using floating point operations.",
}

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

    -- Disable preservation of non-volatile XMM register preservation.
    if _OPTIONS["no-xmm-preserve"] ~= nil then
        defines {
            "SC_NO_XMM_PRESERVE"
        }
    end

    dofile "sc.lua"
    dofile "sc_tests.lua"
