cmake_minimum_required(VERSION 3.26)
project(patcher)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEV_MODE)
endif()

set(MSVC_LIKE OFF)
if (MSVC)
    set(MSVC_LIKE ON)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC" OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        set(MSVC_LIKE ON)
    endif ()
endif ()
if (MSVC_LIKE)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    add_compile_options(/MP /FS /utf-8 /bigobj /GR-)
else()
    add_compile_options(-fno-rtti)
endif()

file(GLOB_RECURSE sourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        libhat_src
        GIT_REPOSITORY https://github.com/BasedInc/libhat.git
        GIT_TAG        v0.4.0
)
FetchContent_MakeAvailable(libhat_src)
if (TARGET libhat::libhat)
    set(LIBHAT_TARGET libhat::libhat)
elseif (TARGET libhat)
    set(LIBHAT_TARGET libhat)
else()
    message(FATAL_ERROR "Cible libhat introuvable après FetchContent.")
endif()

FetchContent_Declare(
        minhook_src
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG        v1.3.4
)
FetchContent_MakeAvailable(minhook_src)
if (TARGET minhook)
    set(MINHOOK_TARGET minhook)
elseif (TARGET MinHook)
    set(MINHOOK_TARGET MinHook)
else()
    FetchContent_GetProperties(minhook_src)
    if (NOT minhook_src_POPULATED)
        FetchContent_Populate(minhook_src)
    endif()
    add_library(minhook STATIC
            "${minhook_src_SOURCE_DIR}/src/buffer.c"
            "${minhook_src_SOURCE_DIR}/src/hook.c"
            "${minhook_src_SOURCE_DIR}/src/trampoline.c"
            "${minhook_src_SOURCE_DIR}/src/hde/hde32.c"
            "${minhook_src_SOURCE_DIR}/src/hde/hde64.c"
            src/Module/Impl/JumpDelay.hpp
    )
    target_include_directories(minhook PUBLIC
            "${minhook_src_SOURCE_DIR}/include"
            "${minhook_src_SOURCE_DIR}/src"
            "${minhook_src_SOURCE_DIR}/src/hde"
    )
    if (MSVC_LIKE)
        target_compile_definitions(minhook PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
    set(MINHOOK_TARGET minhook)
endif()

add_library(patcher SHARED ${sourceFiles})
target_precompile_headers(patcher PRIVATE src/pch.hpp)

target_include_directories(patcher PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        psapi.lib
)

target_link_libraries(patcher PRIVATE
        ${LIBHAT_TARGET}
        ${MINHOOK_TARGET}
        nlohmann_json::nlohmann_json

)

set_target_properties(patcher PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
