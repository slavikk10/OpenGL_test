cmake_minimum_required(VERSION 3.10)
set(CMAKE_OSX_ARCHITECTURES "arm64")
project(OpenGLTest)

set(CMAKE_OSX_SYSROOT "/Users/vyacheslav/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.4.sdk" CACHE STRING "macOS SDK path")
set(CMAKE_XCODE_VERSION 15.4)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

add_executable(OpenGL_test_scene  test.cpp src/glad.c include/SrcXEngine/shader.h stb_image.cpp include/filesystem.h include/SrcXEngine/camera.h include/SrcXEngine/srcx.hpp include/SrcXEngine/mesh.hpp include/SrcXEngine/model.hpp include/imgui/imgui.cpp include/imgui/backends/imgui_impl_glfw.cpp include/imgui/backends/imgui_impl_opengl3.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_tables.cpp include/imgui/imgui_widgets.cpp src/tinyfiledialogs.c)

include_directories(/Users/vyacheslav/.brew/opt/glfw/include)
include_directories(/Users/vyacheslav/.brew/opt/assimp/include)
include_directories(/Users/vyacheslav/OpenGL_C++_test_scene/include)

target_link_libraries(OpenGL_test_scene /Users/vyacheslav/.brew/opt/glfw/lib/libglfw.3.dylib)
target_link_libraries(OpenGL_test_scene /Users/vyacheslav/.brew/opt/assimp/lib/libassimp.5.4.3.dylib)

target_link_libraries(OpenGL_test_scene OpenGL::GL)
