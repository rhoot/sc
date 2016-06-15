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

    -- We're using asm from boost::context. Updating the files every time they
    -- get updated in the repo seems like a pain, so let's just fix the exports
    -- thing with a define...
    defines {
        "BOOST_CONTEXT_EXPORT=_EXPORT"
    }
