set (CMAKE_C_COMPILER "/usr/bin/clang-10")
set (CMAKE_CXX_COMPILER "/usr/bin/clang++-10")

cmake_minimum_required(VERSION 3.10)

# 工程名称
project(LAT)

# 设置
set(CMAKE_CXX_STANDARD 14)


set(CMAKE_ELF_LINKER_FLAGS    "-g")
# set ( CMAKE_VERBOSE_MAKEFILE on  )
# set(LINK_FLAGS "$(-L/usr/lib/llvm-10/lib -lLLVM-10)")

# target_link_libraries("$(llvm-config-10 --libs)") 



include_directories(${CMAKE_SOURCE_DIR}/include /usr/lib/llvm-10/include)
link_directories("/usr/lib/llvm-10/lib")
link_libraries("LLVM-10")
add_compile_options(-std=c++14 --save-temps -fno-rtti -fno-exceptions -g -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)

FILE(GLOB SC_FILES "*.cpp")

# 编译源码生成目标
# add_executable(LAT init.cpp FunctionCallPass.cpp SelectionCountPass.cpp LoopCountPass.cpp FunctionNamePass.cpp)
add_executable(LAT ${SC_FILES})

