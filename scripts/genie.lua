function lib_project(name, fn)
    project(name)
    language "C++"
    targetname(name)
    kind "StaticLib"

    objdir(path.join("../build", _ACTION, "obj"))

    configuration {"x32"}
        targetdir(path.join("../lib", _ACTION, "x32"))

    configuration {"x64"}
        targetdir(path.join("../lib", _ACTION, "x64"))

    configuration {}
end

function exe_project(name, fn)
    lib_project(name)
    kind "ConsoleApp"
    targetdir "../bin"
end

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

        excludes {
            path.join(basepath, "**_gas.asm"),
            sfiles,
        }

    configuration {"vs*", "x32"}
        excludes {path.join(basepath, "**_x86_64_ms_pe_masm.asm")}
        excludes {path.join(basepath, "**_armasm.asm")}

    configuration {"vs*", "x64"}
        excludes {path.join(basepath, "**_i386_ms_pe_masm.asm")}
        excludes {path.join(basepath, "**_armasm.asm")}

    --
    -- Other compilers, only include the relevant files:
    --

    configuration {}
end

dofile "solution.lua"