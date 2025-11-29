cmake_minimum_required(VERSION 3.10.0)
project(dmsh-sfml VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
  sigslot
  GIT_REPOSITORY https://github.com/palacaze/sigslot
  GIT_TAG        19a6f0f5ea11fc121fe67f81fd5e491f2d7a4637 # v1.2.0
)
FetchContent_MakeAvailable(sigslot)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG        55f93686c01528224f448c19128836e7df245f72 # v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.91.1
)

FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  ImGui-SFML
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml
  GIT_TAG        5e8f8fb857c8f3cff1096bf3e4402c64e3bcf6af # v3.0 
)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)

set(SFML_DIR ${SFML_SOURCE_DIR})
#set(SFML_DIR "${CMAKE_BINARY_DIR}/_deps/sfml-src")

FetchContent_MakeAvailable(ImGui-SFML)

add_subdirectory(src)

