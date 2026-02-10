cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)

project(Beuro)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

find_package(nlohmann_json 3.12.0 CONFIG REQUIRED)
find_package(OpenSSL 3.0 REQUIRED)
find_package(Opus CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
add_subdirectory(DPP)
add_subdirectory(SQLiteCpp)
add_subdirectory(chromadb-cpp)
add_subdirectory(dotenv-cpp)

add_executable(Beuro main.cpp src/BeuroAI.cpp src/BeuroDB.cpp src/Admin.cpp)
add_executable(ChromaCLI Tests/Admin.cpp)
add_executable(BeuroDB Tests/BeuroDB.cpp)
add_executable(Beuro-proto Tests/Beuro-proto.cpp src/BeuroDB.cpp src/Admin.cpp)

set_target_properties(Beuro PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(ChromaCLI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(BeuroDB PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(Beuro-proto PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


if (WIN32)
    add_custom_command(TARGET Beuro POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_RUNTIME_DLLS:Beuro>"
            "$<TARGET_FILE_DIR:Beuro>"
        COMMAND_EXPAND_LISTS
    )
endif()

target_link_libraries(Beuro PRIVATE 
    dpp
    dotenv 
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
    Opus::opus
    unofficial-sodium::sodium
    ZLIB::ZLIB
    chromadb
    SQLiteCpp)

target_link_libraries(ChromaCLI PRIVATE
    chromadb
)

target_link_libraries(BeuroDB PRIVATE
    SQLiteCpp    
    dotenv
)

target_link_libraries(Beuro-proto PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    SQLiteCpp
    chromadb
    dotenv
)

target_include_directories(Beuro PRIVATE 
    Header
    chromadb-cpp/include
)

target_include_directories(Beuro-proto PRIVATE
    Header
    chromadb-cpp/include
)

target_include_directories(ChromaCLI PRIVATE
    chromadb-cpp/include
)
