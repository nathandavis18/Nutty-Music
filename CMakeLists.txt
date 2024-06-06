cmake_minimum_required(VERSION 3.26)
project(NuttyMusic LANGUAGES CXX)

set(wxWidgets_USE_STATIC 1)
set(wxBUILD_SHARED OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(wxWidgets
    GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
    GIT_TAG v3.2.1)

FetchContent_GetProperties(wxWidgets)
if(NOT wxwidgets_POPULATED)
   FetchContent_Populate(wxwidgets)
   add_subdirectory(${wxwidgets_SOURCE_DIR} ${wxwidgets_BUILD_DIR})
endif()
FetchContent_MakeAvailable(wxWidgets)


set(SOURCES 
    src/DataProcessing.cpp
    src/mainapp.cpp
    src/mainwindow.cpp
    src/MusicController.cpp
)

set(HEADERS
    Headers/DataProcessing.hpp
    Headers/mainapp.hpp
    Headers/mainwindow.hpp
    Headers/MusicController.hpp
)

add_executable(NuttyMusic WIN32 ${SOURCES})
target_include_directories(NuttyMusic PRIVATE ${wxWidgets_SOURCE_DIR}/include)
target_include_directories(NuttyMusic PRIVATE libs)

target_link_directories(NuttyMusic PRIVATE ${CMAKE_BINARY_DIR}/lib/vc_x64_lib/)

target_link_libraries(NuttyMusic PRIVATE wx::core wx::base wx::net)
target_link_libraries(NuttyMusic PRIVATE ${wxWidgets_LIBRARIES})
target_compile_features(NuttyMusic PRIVATE cxx_std_20)