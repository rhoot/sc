# ---
#  Copyright (c) 2016 Johan Sköld
#  License: https://opensource.org/licenses/ISC
# ---

cmake_minimum_required(VERSION 3.3)
project(sc)
enable_language(C CXX)


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

if(MSVC AND CMAKE_CL_64 EQUAL 0)
    set(CMAKE_ASM_MASM_FLAGS "${CMAKE_ASM_MASM_FLAGS} /safeseh")
endif()

if(CMAKE_OSX_SYSROOT)
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -isysroot ${CMAKE_OSX_SYSROOT}")
endif()

#
# sc
#

add_library(sc STATIC
    include/sc.h
    src/fcontext.h
    src/sc.c)

if(MSVC)
    if(CMAKE_CL_64 EQUAL 0)
        target_sources(sc PRIVATE
            src/asm/jump_i386_ms_pe_masm.asm
            src/asm/make_i386_ms_pe_masm.asm)
    else()
        target_sources(sc PRIVATE
            src/asm/jump_x86_64_ms_pe_masm.asm
            src/asm/make_x86_64_ms_pe_masm.asm)
    endif()
else()
    target_sources(sc PRIVATE
        src/asm/jump_gas.S
        src/asm/make_gas.S)
endif()

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
    tests/win_abi_tests.cpp)

if(MSVC)
    if(CMAKE_CL_64 EQUAL 1)
        target_sources(sc_tests PRIVATE
            tests/asm/get_rsp_proc_x86_64_ms_pe_masm.asm
            tests/asm/get_xmm_register_x86_64_ms_pe_masm.asm
            tests/asm/set_xmm_register_x86_64_ms_pe_masm.asm)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" AND NOT SC_FORCE_32BIT)
        target_sources(sc_tests PRIVATE
            tests/asm/get_rsp_proc_x86_64_ms_pe_gas.s
            tests/asm/get_xmm_register_x86_64_ms_pe_gas.s
            tests/asm/set_xmm_register_x86_64_ms_pe_gas.s)
    endif()
endif()

if(IOS)
    set_target_properties(sc_tests PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "cc.skold.sc_tests"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/scripts/ios-info.plist.in"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "cc.skold.sc_tests")
endif()

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
