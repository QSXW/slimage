cmake_minimum_required(VERSION 2.4)

project(example)

set ( CMAKE_VERBOSE_MAKEFILE on  )

set (CMAKE_COMPILE_OPTIONS -Wall -mwindows -mfma -march=haswell -static)
if(MSVC)
  set(CMAKE_COMPILE_OPTIONS  /Oi /Zc:__cplusplus )
endif()

add_compile_options(${CMAKE_COMPILE_OPTIONS})
link_directories(${PROJECT_SOURCE_DIR}/opengl/lib-mingw-w64)
link_libraries(glfw3)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/opengl/include)
add_executable(example ${PROJECT_SOURCE_DIR}/example.c
    ${PROJECT_SOURCE_DIR}/opengl/src/glad.c
    ${PROJECT_SOURCE_DIR}/src/slbmp.c
    ${PROJECT_SOURCE_DIR}/src/liblog.c
    ${PROJECT_SOURCE_DIR}/src/slendian.c
    ${PROJECT_SOURCE_DIR}/src/sequence.c
    ${PROJECT_SOURCE_DIR}/src/stream.c
    ${PROJECT_SOURCE_DIR}/src/sljpeg.c
    ${PROJECT_SOURCE_DIR}/src/slframe.c
    ${PROJECT_SOURCE_DIR}/src/slcolorspace.c
    ${PROJECT_SOURCE_DIR}/src/slresize.c
    ${PROJECT_SOURCE_DIR}/src/sldct.c
    ${PROJECT_SOURCE_DIR}/src/slimage.c
    ${PROJECT_SOURCE_DIR}/src/slchecksum.c
)
