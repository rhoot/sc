project "sc_tests"
    language "C++"
    targetname "sc_tests"
    toolchain "ConsoleApp"

    includedirs {
        "../3rdparty/Catch/include",
    }

    files {
        "../tests/**.hpp",
        "../tests/**.cpp",
    }

    links {
        "sc",
    }
