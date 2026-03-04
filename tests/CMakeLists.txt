
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SrcFiles
        ${MY_SRC_DIR}/ConverterJSON.cpp
        ${MY_SRC_DIR}/InvertedIndex.cpp
        ${MY_SRC_DIR}/SearchServer.cpp
        ${MY_INCLUDE_DIR}/ConverterJSON.h
        ${MY_INCLUDE_DIR}/InvertedIndex.h
        ${MY_INCLUDE_DIR}/SearchServer.h
)

set(TestsFiles SearchEngineTest.cpp)

add_executable(GtestTests ${TestsFiles} ${SrcFiles})

add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)
target_link_libraries(GtestTests
        PRIVATE
        GTest::GTest
        nlohmann_json::nlohmann_json)

include(GoogleTest)
target_include_directories(GtestTests PUBLIC ${MY_INCLUDE_DIR})
gtest_discover_tests(GtestTests)