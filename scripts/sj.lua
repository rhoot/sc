lib_project "sj"
    flags {
        "NoRTTI",
    }

    files {
        "../include/*.h",
        "../include/**.hpp",
        "../src/**.hpp",
        "../src/**.cpp",
    }

    asm_files "../src"
