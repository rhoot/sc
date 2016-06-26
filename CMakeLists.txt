cmake_minimum_required(VERSION 3.4)
project(sc)
enable_language(C CXX)

# == target cpu == #

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(MSVC)
        if(CMAKE_CL_64 EQUAL 0)
            set(TARGET_CPU "i386")
        elseif(CMAKE_CL_64 EQUAL 1)
            set(TARGET_CPU "x86_64")
        elseif(CMAKE_CL_64 EQUAL 2)
            set(TARGET_CPU "arm")
        endif()
    else()
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
            set(TARGET_CPU "x86_64")
        else()
            set(TARGET_CPU "i386")
        endif()
    endif()
else()
    set(TARGET_CPU ${CMAKE_SYSTEM_PROCESSOR})
endif()

message("Target: ${CMAKE_SYSTEM_NAME}-${TARGET_CPU}")

# == assembly suffix == #

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(MSVC)
        enable_language(ASM_MASM)
        set(ASSEMBLY_SUFFIX "_ms_pe_masm.asm")
    else()
        enable_language(ASM-ATT)
        set(ASSEMBLY_SUFFIX "_ms_pe_gas.s")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    enable_language(ASM-ATT)
    set(ASSEMBLY_SUFFIX "_sysv_macho_gas.s")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    enable_language(ASM-ATT)
    set(ASSEMBLY_SUFFIX "_sysv_elf_gas.s")
else()
    message(FATAL_ERROR "unsupported system")
endif()

# == compiler flags == #

if(MSVC)
    set(CMAKE_ASM_MASM_FLAGS  "${CMAKE_ASM_MASM_FLAGS} /W3 /WX")
    set(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} /W4 /WX")
    set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
    set(CMAKE_ASM-ATT_FLAGS   "${CMAKE_ASM-ATT_FLAGS} -Wall -Werror")
    set(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -Wall -Werror -std=c89")
    set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++0x")
endif()

add_definitions(-DBOOST_CONTEXT_EXPORT=)

# == output paths == #

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR})

# == files == #

include_directories(include)

file(GLOB_RECURSE SC_SOURCES        src/*.c src/*.h include/*.h src/*_${TARGET_CPU}${ASSEMBLY_SUFFIX})
file(GLOB_RECURSE SC_TESTS_SOURCES  tests/*.cpp tests/*.hpp tests/*_${TARGET_CPU}${ASSEMBLY_SUFFIX})

# == targets == #

add_library(sc STATIC ${SC_SOURCES})
add_executable(sc_tests ${SC_TESTS_SOURCES})

target_include_directories(sc_tests PUBLIC "3rdparty/Catch/include")
target_link_libraries(sc_tests sc)
