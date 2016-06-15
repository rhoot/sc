exe_project "sj_tests"
    includedirs {
        "../3rdparty/Catch/include",
    }

    files {
        "../tests/**.hpp",
        "../tests/**.cpp",
    }

    links {
        "sj",
        "winmm",
    }
