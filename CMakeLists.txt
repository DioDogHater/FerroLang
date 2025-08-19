cmake_minimum_required(VERSION 3.20)

project(FerroLang)

# Debug build
set(CMAKE_BUILD_TYPE Debug)

# FerroLang library
add_library(FL STATIC
	src/FL/filemanager.c
	src/FL/tokenizer.c
	src/FL/datastructures.c
)

# FerroLang interpreter
add_executable(ferro_interpreter src/interpreter.c)
target_link_libraries(ferro_interpreter FL)

# FerroLang compiler
#add_executable(ferro_compiler)