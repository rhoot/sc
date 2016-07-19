function sc.on_solution()
    platforms {"ARM"}
    removeplatforms {"Universal", "x32", "x64"}
end

premake.gcc.platforms.ARM = {
    ar       = "/rpxc/bin/arm-linux-gnueabihf-ar",
    cc       = "/rpxc/bin/arm-linux-gnueabihf-gcc",
    cxx      = "/rpxc/bin/arm-linux-gnueabihf-g++",
    cppflags = "-MMD -MP",
    flags    = "--sysroot=/rpxc/sysroot",
}
