cmake_minimum_required(VERSION 3.28)
# Name is Game07 'coz of the number of the lesson
project(Game07 LANGUAGES CXX)

# require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# ensure -std=c++XX instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-reorder")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-reorder")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
#option(SFML_BUILD_AUDIO "Build audio" OFF)
#option(SFML_BUILD_NETWORK "Build network" OFF)
FetchContent_MakeAvailable(SFML)

## Adding imggui and imgui-sfml
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG     v1.91.0)

# Or by tag...
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG        2.6.x
    # GIT_COMMIT 5f54b69b6aee18db846c81633f86f78c2586dded
    # ^ or like this - sometimes it's better because FetchContent won't look
    # into remote to see if branch head was updated or not - good for stable
    # tags like 'vX.X' corresponding to releases
)

## finally, let's build
# Dear ImGui
FetchContent_MakeAvailable(imgui)

# ImGui-SFML
set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
#option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)

FetchContent_MakeAvailable(imgui-sfml)

include_directories(include)
include_directories(assets)

add_executable(${PROJECT_NAME}
    main.cpp
    include/Game.h
    include/Components.h
    include/Vec2.h
    include/Entity.h
    include/EntityManager.h
    include/FileUtils.h
    src/Game.cpp
    src/Vec2.cpp
    src/Entity.cpp
    src/EntityManager.cpp
    src/FileUtils.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC ImGui-SFML::ImGui-SFML)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM)
endif()


file(
    COPY ${CMAKE_SOURCE_DIR}/assets/data/config.txt
    DESTINATION ${CMAKE_BINARY_DIR}/assets/data
)

file(GLOB fonts ${CMAKE_SOURCE_DIR}/assets/fonts/*.ttf)
file(COPY ${fonst} DESTINATION ${CMAKE_BINARY_DIR}/assets/fonts)
