cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)

project(Beuro)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

find_package(SQLiteCpp CONFIG REQUIRED)
find_package(nlohmann_json 3.12.0 CONFIG REQUIRED)
find_package(OpenSSL 3.0 REQUIRED)
find_package(Opus CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
add_subdirectory(DPP)
add_subdirectory(chromadb-cpp)
add_subdirectory(dotenv-cpp)


add_executable(Beuro main.cpp src/BeuroAI.cpp)
add_executable(ChromaCLI src/Admin.cpp)
add_executable(BeuroDB src/BeuroDB.cpp)
set_target_properties(Beuro PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(ChromaCLI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(BeuroDB PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (WIN32)
    add_custom_command(TARGET Beuro POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_RUNTIME_DLLS:Beuro>"
            "$<TARGET_FILE_DIR:Beuro>"
        COMMAND_EXPAND_LISTS
    )
endif()

target_precompile_headers(Beuro PRIVATE
    <fstream>
    <dpp/dpp.h>
    <iostream>
    <nlohmann/json.hpp>
)

target_link_libraries(Beuro PRIVATE 
    dpp
    dotenv 
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
    Opus::opus
    unofficial-sodium::sodium
    ZLIB::ZLIB)

target_link_libraries(ChromaCLI PRIVATE
    chromadb
)

target_link_libraries(BeuroDB PRIVATE
    SQLiteCpp
)

target_include_directories(Beuro PRIVATE 
    Header
)

target_include_directories(ChromaCLI PRIVATE
    chromadb-cpp/include
)
