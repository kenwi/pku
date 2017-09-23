cmake_minimum_required(VERSION 3.5)
project(pku)

set(MAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(pku ${SOURCE_FILES})
target_link_libraries(pku pthread csfml-graphics csfml-window csfml-system)