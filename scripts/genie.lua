newoption {
    trigger = "build-dir",
    description = "Build directory, relative to genie.lua",
}

newoption {
    trigger = "toolchain",
    description = "Toolchain configuration file",
}

-- Who the hell uses a ppc mac these days?!
premake.gcc.platforms.Universal.flags = "-arch i386 -arch x86_64"
premake.gcc.platforms.Universal32.flags = "-arch i386"
premake.gcc.platforms.Universal64.flags = "-arch x86_64"

-- The build directory should be relative to genie.lua
local sc_builddir = "../build"

if _OPTIONS["build-dir"] then
    print("Building to: " .. _OPTIONS["build-dir"])
    sc_builddir = path.getrelative(os.getcwd(), path.join(_WORKING_DIR, _OPTIONS["build-dir"]))
end

-- By overriding these functions in a toolchain file, you can customize the
-- build for each solution or project.
sc = {
    on_solution = function() end,
    on_project = function() end,
}

-- Toolchain file support (external overriding of compiler options).
if _OPTIONS.toolchain then
    print("Using toolchain file: " .. _OPTIONS.toolchain)
    dofile(path.getrelative(os.getcwd(), path.join(_WORKING_DIR, _OPTIONS.toolchain)))
end

solution "sc"
    sc.on_solution()
    configuration {}

    if #(solution().configurations) == 0 then
        configurations {
            "Debug",
            "Release",
        }
    end

    if #(solution().platforms) == 0 then
        if os.is("MacOSX") then
            platforms {"Universal", "x32", "x64"}
        else
            platforms {"x32", "x64"}
        end
    end

    flags {
        "ExtraWarnings",
        "FatalWarnings",
        "NoPCH",
        "StaticRuntime",
        "Symbols",
    }

    includedirs {
        "../include",
    }

    configuration {"gmake"}
        buildoptions_c {"-std=c89"}
        buildoptions_cpp {"-std=c++0x"}

    location(sc_builddir)
    objdir(path.join(sc_builddir, "obj"))

    startproject "sc_tests"

    project "sc"
        language "C"
        kind "StaticLib"
        targetname "sc"
        targetdir(path.join(sc_builddir, "lib"))

        sc.on_project()
        configuration {}

        defines {
            "BOOST_CONTEXT_EXPORT=_EXPORT"
        }

        flags {
            "NoExceptions",
            "NoRTTI",
        }

        files {
            "../include/**.h",
            "../src/**.h",
            "../src/**.c",
        }

        configuration {"vs*", "x32"}
            files {"../src/**_i386_ms_pe_masm.asm"}

        configuration {"vs*", "x64"}
            files {"../src/**_x86_64_ms_pe_masm.asm"}

        configuration {"not vs*"}
            files {"../src/asm/jump_gas.S"}
            files {"../src/asm/make_gas.S"}

    project "sc_tests"
        language "C++"
        kind "ConsoleApp"
        targetname "sc_tests"
        targetdir(path.join(sc_builddir, "bin"))

        sc.on_project()
        configuration {}

        includedirs {
            "../3rdparty/Catch/include",
        }

        files {
            "../tests/**.hpp",
            "../tests/**.cpp",
        }

        links {
            "sc"
        }

        configuration {"vs*", "x64"}
            files {"../tests/**_x86_64_ms_pe_masm.asm"}

        configuration {"gmake"}
            files {"../tests/**_gas.S"}
