cmake_minimum_required(VERSION 3.16)

# Sets project name
project(FinalProject LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/glrenderer.cpp
    src/mainwindow.cpp

    src/glrenderer.h
    src/mainwindow.h
    src/shaderloader.h
    src/aspectratiowidget/aspectratiowidget.hpp
    src/camera.cpp
    src/bezier.cpp
    src/city.cpp
    src/shapes.cpp
)
# Specifies other files
qt_add_resources(${PROJECT_NAME} "Resources"
  PREFIX
  "/"
  FILES
  resources/textures/facade_diffuse1.png
    resources/textures/facade_diffuse2.png
    resources/textures/facade_diffuse3.png
    resources/textures/facade_diffuse4.png
    resources/textures/facade_diffuse5.png
    resources/textures/facade_diffuse6.png
    resources/textures/facade_diffuse7.png
    resources/textures/facade_diffuse8.png
    resources/textures/facade_diffuse9.png
    resources/textures/facade_diffuse10.png
    resources/textures/facade_diffuse11.png
    resources/textures/facade_normal1.png
    resources/textures/facade_normal2.png
    resources/textures/facade_normal3.png
    resources/textures/facade_normal4.png
    resources/textures/facade_normal5.png
    resources/textures/facade_normal6.png
    resources/textures/facade_normal7.png
    resources/textures/facade_normal8.png
    resources/textures/facade_normal9.png
    resources/textures/facade_normal10.png
    resources/textures/facade_normal11.png
  resources/textures/road_diffuse.jpeg
  resources/textures/road_normal.png
  resources/textures/grass_diffuse.png
  resources/textures/grass_normal.png
  resources/shaders/default.vert
  resources/shaders/default.frag
  resources/shaders/shadow.vert
  resources/shaders/shadow.frag
)



# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::OpenGL
  Qt::OpenGLWidgets
  Qt::Gui
  StaticGLEW
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()

# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
