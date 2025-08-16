cmake_minimum_required(VERSION 3.5)

project(FerroLang)

# FerroLang library
add_library(FL STATIC
	src/FL/filemanager.c
	src/FL/tokenizer.c
)

# FerroLang interpreter
add_executable(ferro_interpreter src/interpreter.c)
target_link_libraries(ferro_interpreter FL)

# FerroLang compiler
#add_executable(ferro_compiler)