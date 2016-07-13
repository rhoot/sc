# ---
#  Copyright (c) 2016 Johan Sköld
#  License: https://opensource.org/licenses/ISC
# ---

set(IOS ON)

if(NOT DEFINED IOS_ARCHITECTURES)
    set(IOS_ARCHITECTURES armv7 armv7s arm64)
endif()

set(CMAKE_OSX_ARCHITECTURES ${IOS_ARCHITECTURES} CACHE string  "Build architecture for iOS")

set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos")

if(NOT DEFINED IOS_DEVELOPER_PATH)
    set(IOS_DEVELOPER_PATH "/Applications/Xcode.app/Contents/Developer")
endif()

if(NOT EXISTS ${IOS_DEVELOPER_PATH})
    message(FATAL_ERROR "Could not find '${IOS_DEVELOPER_PATH}'. Please set IOS_DEVELOPER_PATH to a valid path.")
endif()

set(CMAKE_ASM_COMPILER /usr/bin/gcc)
set(CMAKE_C_COMPILER   /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Skip the platform compiler checks
set(CMAKE_ASM_COMPILER_WORKS TRUE)
set(CMAKE_C_COMPILER_WORKS   TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# iOS SDK
set(CMAKE_FIND_ROOT_PATH ${IOS_DEVELOPER_PATH}/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS9.3.sdk)
set(CMAKE_IOS_SDK_PATH   ${IOS_DEVELOPER_PATH}/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS9.3.sdk)
set(CMAKE_OSX_SYSROOT    ${IOS_DEVELOPER_PATH}/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS9.3.sdk)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
