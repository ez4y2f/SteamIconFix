cmake_minimum_required(VERSION 3.27)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(_VCPKG_CANDIDATE "$ENV{VCPKG_ROOT}")
    else()
        if(EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(_VCPKG_CANDIDATE "C:/vcpkg")
        elseif(DEFINED ENV{USERPROFILE})
            set(_SCOOP_VCPKG_DIR "$ENV{USERPROFILE}/scoop/apps/vcpkg/current")
            if(EXISTS "${_SCOOP_VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")
                set(_VCPKG_CANDIDATE "${_SCOOP_VCPKG_DIR}")
            endif()
        endif()
    endif()

    if(DEFINED _VCPKG_CANDIDATE)
        set(CMAKE_TOOLCHAIN_FILE
                "${_VCPKG_CANDIDATE}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "" FORCE)
        message(STATUS "Using vcpkg toolchain from: ${CMAKE_TOOLCHAIN_FILE}")
    else()
        message(STATUS "vcpkg not found automatically (no VCPKG_ROOT, C:/vcpkg 或 scoop vcpkg)")
    endif()
endif()

set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "")

project(SteamIconFix)
set(CMAKE_CXX_STANDARD 23)

find_package(CURL REQUIRED)

add_executable(SteamIconFix
        SteamIconFix.cpp)

if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

target_link_libraries(SteamIconFix PRIVATE CURL::libcurl)