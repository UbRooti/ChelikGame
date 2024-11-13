cmake_minimum_required(VERSION 3.16)

project(Freaktoria)

set(SRC_FILES

    src/main.cpp
    src/GUI.cpp                 src/GUI.h
    src/Consts.cpp              src/Consts.h
    src/JSON.hpp
    src/Globals.h
    src/ResourceManager.cpp     src/ResourceManager.h
    src/Game/Game.cpp           src/Game/Game.h
    src/Screens/Screen.h

)




set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main ${SRC_FILES})

target_link_libraries(main PRIVATE sfml-graphics sfml-audio sfml-window sfml-system)
target_compile_features(main PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()