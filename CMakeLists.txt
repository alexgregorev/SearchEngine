cmake_minimum_required(VERSION 3.20)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(SearchEngine
    VERSION 4.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

############################################################
# Dependencies
############################################################

# httplib
FetchContent_Declare(
    httplib
    URL https://github.com/yhirose/cpp-httplib/archive/refs/heads/master.zip
)
FetchContent_MakeAvailable(httplib)

# json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/archive/refs/heads/develop.zip
)
FetchContent_MakeAvailable(json)

# googletest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

############################################################
# Subdirectories
############################################################

add_subdirectory(src)
add_subdirectory(tests)