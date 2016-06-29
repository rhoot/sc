cmake_minimum_required(VERSION 3.3)
project(sc)
enable_language(C CXX)


#
# Normalized target CPU
#

if(SC_FORCE_32BIT)
    set(SC_TARGET_CPU "i386")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(MSVC)
        if(CMAKE_CL_64 EQUAL 0)
            set(SC_TARGET_CPU "i386")
        elseif(CMAKE_CL_64 EQUAL 1)
            set(SC_TARGET_CPU "x86_64")
        elseif(CMAKE_CL_64 EQUAL 2)
            set(SC_TARGET_CPU "arm")
        endif()
    elseif(NOT SC_TARGET_CPU)
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
            set(SC_TARGET_CPU "x86_64")
        else()
            set(SC_TARGET_CPU "i386")
        endif()
    endif()
elseif(NOT SC_TARGET_CPU)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
        set(SC_TARGET_CPU "arm")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i686")
        set(SC_TARGET_CPU "i386")
    else()
        set(SC_TARGET_CPU ${CMAKE_SYSTEM_PROCESSOR})
    endif()
endif()


#
# Compiler options
#

if(MSVC)
    enable_language(ASM_MASM)
else()
    enable_language(ASM)
endif()

# For OSX, CMake already handles passing the appropriate `-arch` parameters
# for C/C++, but we need to do it ourselves for assembly files. Due to this
# bug however we must apply it globally through `CMAKE_ASM_FLAGS`:
# https://cmake.org/Bug/view.php?id=15826

if(CMAKE_OSX_ARCHITECTURES)
    string(REPLACE ";" " -arch " SC_OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}")
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -arch ${SC_OSX_ARCHITECTURES}")
endif()

# Windows requires the /safeseh flag to be passed to 32-bit MASM files.
# Something CMake doesn't do by default. This must be done globally as
# `/safeseh` is only used for assembly (not C), and there is no support for
# `$<COMPILE_LANGUAGE:...>` in Visual Studio.

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND SC_TARGET_CPU STREQUAL "i386")
    set(CMAKE_ASM_MASM_FLAGS "${CMAKE_ASM_MASM_FLAGS} /safeseh")
endif()

#
# sc
#

add_library(sc STATIC
    include/sc.h
    src/fcontext.h
    src/sc.c

    # Windows (MSVC)
    $<$<C_COMPILER_ID:MSVC>:src/asm/jump_${SC_TARGET_CPU}_ms_pe_masm.asm>
    $<$<C_COMPILER_ID:MSVC>:src/asm/make_${SC_TARGET_CPU}_ms_pe_masm.asm>

    # Windows (MinGW/MSYS)
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<C_COMPILER_ID:MSVC>>>:src/asm/jump_${SC_TARGET_CPU}_ms_pe_gas.s>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<C_COMPILER_ID:MSVC>>>:src/asm/make_${SC_TARGET_CPU}_ms_pe_gas.s>

    # OSX
    $<$<PLATFORM_ID:Darwin>:src/asm/jump_i386_x86_64_sysv_macho_gas.s>
    $<$<PLATFORM_ID:Darwin>:src/asm/make_i386_x86_64_sysv_macho_gas.s>

    # Linux
    $<$<PLATFORM_ID:Linux>:src/asm/jump_${SC_TARGET_CPU}_sysv_elf_gas.s>
    $<$<PLATFORM_ID:Linux>:src/asm/make_${SC_TARGET_CPU}_sysv_elf_gas.s>

    # Android (arm)
    $<$<AND:$<PLATFORM_ID:Android>,$<STREQUAL:${SC_TARGET_CPU},arm>>:src/asm/jump_arm_aapcs_elf_gas.s>
    $<$<AND:$<PLATFORM_ID:Android>,$<STREQUAL:${SC_TARGET_CPU},arm>>:src/asm/make_arm_aapcs_elf_gas.s>

    # Android (mips)
    $<$<AND:$<PLATFORM_ID:Android>,$<STREQUAL:${SC_TARGET_CPU},mips>>:src/asm/jump_mips32_o32_elf_gas.s>
    $<$<AND:$<PLATFORM_ID:Android>,$<STREQUAL:${SC_TARGET_CPU},mips>>:src/asm/make_mips32_o32_elf_gas.s>

    # Android (x86)
    $<$<AND:$<PLATFORM_ID:Android>,$<STREQUAL:${SC_TARGET_CPU},i386>>:src/asm/jump_i386_sysv_elf_gas.s>
    $<$<AND:$<PLATFORM_ID:Android>,$<STREQUAL:${SC_TARGET_CPU},i386>>:src/asm/make_i386_sysv_elf_gas.s>

    # Android (x86_64)
    $<$<AND:$<PLATFORM_ID:Android>,$<STREQUAL:${SC_TARGET_CPU},x86_64>>:src/asm/jump_x86_64_sysv_elf_gas.s>
    $<$<AND:$<PLATFORM_ID:Android>,$<STREQUAL:${SC_TARGET_CPU},x86_64>>:src/asm/make_x86_64_sysv_elf_gas.s>)

target_compile_definitions(sc
    PRIVATE
        BOOST_CONTEXT_EXPORT=)

target_compile_options(sc
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wall -Werror>
        $<$<C_COMPILER_ID:Clang>:-Wall -Werror>
        $<$<C_COMPILER_ID:AppleClang>:-Wall -Werror>
        $<$<C_COMPILER_ID:MSVC>:/W3 /WX>)

target_include_directories(sc
    PUBLIC include)


#
# sc_tests
#

add_executable(sc_tests
    tests/framework.hpp
    tests/main.cpp
    tests/sc_tests.cpp
    tests/win_abi_tests.cpp

    # Windows (MSVC, x86_64)
    $<$<AND:$<C_COMPILER_ID:MSVC>,$<STREQUAL:${SC_TARGET_CPU},x86_64>>:tests/asm/get_rsp_proc_x86_64_ms_pe_masm.asm>
    $<$<AND:$<C_COMPILER_ID:MSVC>,$<STREQUAL:${SC_TARGET_CPU},x86_64>>:tests/asm/get_xmm_register_x86_64_ms_pe_masm.asm>
    $<$<AND:$<C_COMPILER_ID:MSVC>,$<STREQUAL:${SC_TARGET_CPU},x86_64>>:tests/asm/set_xmm_register_x86_64_ms_pe_masm.asm>

    # Windows (MinGW/MSYS, x86_64)
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<C_COMPILER_ID:MSVC>>,$<STREQUAL:${SC_TARGET_CPU},x86_64>>:tests/asm/get_rsp_proc_x86_64_ms_pe_gas.s>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<C_COMPILER_ID:MSVC>>,$<STREQUAL:${SC_TARGET_CPU},x86_64>>:tests/asm/get_xmm_register_x86_64_ms_pe_gas.s>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<C_COMPILER_ID:MSVC>>,$<STREQUAL:${SC_TARGET_CPU},x86_64>>:tests/asm/set_xmm_register_x86_64_ms_pe_gas.s>)

target_compile_options(sc_tests
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Werror -std=c++0x>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Werror -std=c++0x>
        $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Werror -std=c++0x>
        $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>)

target_include_directories(sc_tests
    PRIVATE
        "3rdparty/Catch/include")

target_link_libraries(sc_tests
    sc)


#
# Common properties
#

set_target_properties(sc sc_tests PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(SC_FORCE_32BIT)
    set_target_properties(sc sc_tests PROPERTIES
        COMPILE_FLAGS "-m32"
        LINK_FLAGS "-m32")
endif()
