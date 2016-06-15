exe_project "sc_tests"
    includedirs {
        "../3rdparty/Catch/include",
    }

    files {
        "../tests/**.hpp",
        "../tests/**.cpp",
    }

    links {
        "sc",
        "winmm",
    }
