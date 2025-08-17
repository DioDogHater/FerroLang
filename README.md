# The Ferro Programming Language
This is a strongly typed progamming language
(you have to declare the type of variables and data)
like C, but with more built-in features. This is
probably not a polished language, since this is still
kind of a prototype and not a complete project.

# What it aims to do
This is a language that combines the low level features
of C and the useful features of higher level languages
like easy string operations, dynamic arrays and safe
memory management. This language is also designed to be
interpreted and compiled, making it useful for scripting
and fully fledged performant code.

# How to build / use
```
git clone https://github.com/DioDogHater/FerroLang
cd FerroLang
cmake -S . -B ./build
cmake --build ./build
```
Now, there should be two executables:
- `build/ferro_interpreter`
- `build/ferro_compiler`