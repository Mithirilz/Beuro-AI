cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)


project(Beuro)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(Beuro main.cpp src/BeuroAI.cpp src/Admin.cpp src/BeuroDB.cpp)

set_target_properties(Beuro PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(nlohmann_json 3.12.0 REQUIRED)
find_package(OpenSSL 3.0 REQUIRED)
find_package(Opus CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
add_subdirectory(DPP)
add_subdirectory(SQLiteCpp)
add_subdirectory(chromadb-cpp)
add_subdirectory(dotenv-cpp)



target_link_libraries(Beuro PRIVATE 
    dpp
    dotenv 
    SQLiteCpp
    chromadb
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
    Opus::opus
    unofficial-sodium::sodium
    ZLIB::ZLIB
)

target_include_directories(Beuro PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Header
    ${CMAKE_CURRENT_SOURCE_DIR}/chromadb-cpp/include
)