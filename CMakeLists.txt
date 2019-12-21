cmake_minimum_required(VERSION 2.6)
project(libco)


# 指定语言
enable_language(C CXX ASM)


include_directories(./src)

add_subdirectory(./src)
add_subdirectory(./test)
