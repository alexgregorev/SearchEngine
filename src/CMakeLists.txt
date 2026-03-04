
set(Src
        main.cpp
        ConverterJSON.cpp
        InvertedIndex.cpp
        SearchServer.cpp
        ${MY_INCLUDE_DIR}/ConverterJSON.h
        ${MY_INCLUDE_DIR}/InvertedIndex.h
        ${MY_INCLUDE_DIR}/SearchServer.h
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(SearchEngine ${Src})

target_link_libraries(SearchEngine PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(SearchEngine PUBLIC ${MY_INCLUDE_DIR})