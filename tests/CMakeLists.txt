add_executable(SearchEngineTests
    SearchEngineTests.cpp
)

target_link_libraries(SearchEngineTests
    PRIVATE
        SearchEngineLib
        gtest_main
)

target_include_directories(SearchEngineTests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

include(GoogleTest)

gtest_discover_tests(SearchEngineTests) 