cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(GehennaServer
        src/Main.cpp
        src/Server.cpp
        src/Server.h
        src/backend/PacketHandler.h
        src/backend/PacketHandler.cpp
        src/backend/DataHandler.h
        src/backend/DataHandler.cpp)

# Add SQLite3 library
set(SQLITE3_DIR "C:/Users/laure/Documents/SQLite/sqlite-dll-win-x64-3460000")

# Include directories for SQLite3
target_include_directories(GehennaServer PRIVATE ${SQLITE3_DIR}/include)

# Link directories for SQLite3
target_link_directories(GehennaServer PRIVATE ${SQLITE3_DIR}/lib)

target_link_libraries(GehennaServer PRIVATE sfml-network sqlite3)

target_compile_features(GehennaServer PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
            TARGET GehennaServer
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:GehennaServer>
            VERBATIM)
    add_custom_command(
            TARGET GehennaServer
            COMMENT "Copy SQLite3 DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SQLITE3_DIR}/sqlite3.dll $<TARGET_FILE_DIR:GehennaServer>
            VERBATIM)
endif()