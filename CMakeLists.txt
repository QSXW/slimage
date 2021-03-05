cmake_minimum_required(VERSION 2.4)

project(example)

set ( CMAKE_VERBOSE_MAKEFILE on  )

add_compile_options(-Wall -mwindows -mfma -march=haswell -static)
link_directories(${PROJECT_SOURCE_DIR}/opengl/lib-mingw-w64)
link_libraries(glfw3)

include_directories(${PROJECT_SOURCE_DIR}/slimage/include)
include_directories(${PROJECT_SOURCE_DIR}/opengl/include)
add_executable(example example.c
    ${PROJECT_SOURCE_DIR}/opengl/src/glad.c
    ${PROJECT_SOURCE_DIR}/slimage/src/slbmp.c
    ${PROJECT_SOURCE_DIR}/slimage/src/liblog.c
    ${PROJECT_SOURCE_DIR}/slimage/src/slendian.c
    ${PROJECT_SOURCE_DIR}/slimage/src/sequence.c
    ${PROJECT_SOURCE_DIR}/slimage/src/stream.c
    ${PROJECT_SOURCE_DIR}/slimage/src/sljpeg.c
    ${PROJECT_SOURCE_DIR}/slimage/src/slframe.c
    ${PROJECT_SOURCE_DIR}/slimage/src/slcolorspace.c
    ${PROJECT_SOURCE_DIR}/slimage/src/slresize.c
    ${PROJECT_SOURCE_DIR}/slimage/src/sldct.c
    ${PROJECT_SOURCE_DIR}/slimage/src/slimage.c
)
