cmake_minimum_required(VERSION 3.20)
project(Kotatsu_Internal VERSION 0.1.0 LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c)

add_library(Kotatsu_Internal SHARED ${SRC_FILES})

target_include_directories(Kotatsu_Internal PUBLIC $ENV{DXSDK_DIR}/Include)
target_link_directories(Kotatsu_Internal PUBLIC $ENV{DXSDK_DIR}/Lib/x64)
target_include_directories(Kotatsu_Internal PUBLIC src/imgui)
target_include_directories(Kotatsu_Internal PUBLIC src/kiero)