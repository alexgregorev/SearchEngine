cmake_minimum_required(VERSION 3.30)
project(SearchEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

set(MY_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resourses)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

add_subdirectory(src)
add_subdirectory(tests)