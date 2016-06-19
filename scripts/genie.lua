newoption {
    trigger     = "gcc",
    value       = "GCC",
    description = "GCC flavor",

    allowed = {
        {"mingw", "MinGW (x86, x86_64)"},
        {"linux", "Linux (x86, x86_64)"},
    },
}

function asm_files(basepath)
    local asmfiles = path.join(basepath, "**.asm")
    local sfiles = path.join(basepath, "**.S")

    --
    -- VS: Include all files and exclude irrelevant files:
    --

    configuration {"vs*"}
        files {
            asmfiles,
            sfiles,
        }

        excludes {sfiles}

    configuration {"vs*", "x32"}
        excludes {path.join(basepath, "**_x86_64_ms_pe_masm.asm")}
        excludes {path.join(basepath, "**_armasm.asm")}

    configuration {"vs*", "x64"}
        excludes {path.join(basepath, "**_i386_ms_pe_masm.asm")}
        excludes {path.join(basepath, "**_armasm.asm")}

    --
    -- Other compilers, only include the relevant files:
    --

    configuration {"gmake", "x32"}
        local masks = {
            mingw = "**_i386_ms_pe_gas.S",
            linux = "**_i386_sysv_elf_gas.S",
        }

        if masks[_OPTIONS["gcc"]] then
            files {path.join(basepath, masks[_OPTIONS["gcc"]])}
        end

    configuration {"gmake", "x64"}
        local masks = {
            mingw = "**_x86_64_ms_pe_gas.S",
            linux = "**_x86_64_sysv_elf_gas.S",
        }

        if masks[_OPTIONS["gcc"]] then
            files {path.join(basepath, masks[_OPTIONS["gcc"]])}
        end

    configuration {}
end

function full_action()
    -- _ACTION is nil when running --help
    if _ACTION == nil then
        return nil
    end

    local pathsuffix = ""

    configuration {"gmake"}
        if _OPTIONS.gcc then
            pathsuffix = "-gcc-" .. _OPTIONS.gcc
        end

    configuration {}

    return _ACTION .. pathsuffix
end

function set_location()
    local action = full_action()

    location(path.join("../build", action))
    objdir(path.join("../build", action, "obj"))
end

function toolchain(prjkind)
    local action = full_action()

    kind(prjkind)

    --
    -- Compiler settings
    --

    configuration {"gmake"}
        if _OPTIONS["gcc"] == "mingw" then
            premake.gcc.cc  = "x86_64-w64-mingw32-gcc"
            premake.gcc.cxx = "x86_64-w64-mingw32-g++"
            premake.gcc.ar  = "ar"
        end

        buildoptions_c {
            "-std=c89",
        }

        buildoptions_cpp {
            "-std=c++0x",
        }

    configuration {"vs*"}
        buildoptions {
            "/wd4127", -- C4127: conditional expression is constant

            -- Caused by VC standard headers when exceptions are disabled:
            "/wd4577", -- C4577: 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
        }

    --
    -- Configuration settings
    --

    configuration {"Release"}
        defines {
            "NDEBUG"
        }

    --
    -- Paths
    --

    configuration {}
        includedirs {"../include"}

    configuration {"x32"}
        libdirs {path.join("../lib", action, "x32")}

        if prjkind == "ConsoleApp" or prjkind == "WindowedApp" then
            targetdir "../bin"
        else
            targetdir(path.join("../lib", action, "x32"))
        end

    configuration {"x64"}
        libdirs {path.join("../lib", action, "x64")}

        if prjkind == "ConsoleApp" or prjkind == "WindowedApp" then
            targetdir "../bin"
        else
            targetdir(path.join("../lib", action, "x64"))
        end

    configuration {}
end

dofile "solution.lua"