############################################################
# Library
############################################################

add_library(SearchEngineLib
    InvertedIndex.cpp
    JsonStorage.cpp
    SearchServer.cpp
    TextUtils.cpp
    WebServer.cpp
)

target_include_directories(SearchEngineLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${httplib_SOURCE_DIR}
)

target_link_libraries(SearchEngineLib
    PUBLIC
        nlohmann_json::nlohmann_json
)

############################################################
# Compiler warnings
############################################################

if (MSVC)
    target_compile_options(SearchEngineLib PRIVATE /W4)
else()
    target_compile_options(SearchEngineLib PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

############################################################
# Executable
############################################################

add_executable(SearchEngine
    main.cpp
)

target_link_libraries(SearchEngine
    PRIVATE
    SearchEngineLib
)

set_target_properties(SearchEngine
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)