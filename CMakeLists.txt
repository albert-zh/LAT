set (CMAKE_C_COMPILER "/usr/bin/clang-10")
set (CMAKE_CXX_COMPILER "/usr/bin/clang++-10")

cmake_minimum_required(VERSION 3.10)

# 工程名称
project(LAT)

# 设置
set(CMAKE_CXX_STANDARD 14)
set(COMPILE_FLAGS "-std=c++14   -fno-exceptions -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
# set(LINK_FLAGS "$(-L/usr/lib/llvm-10/lib -lLLVM-10)")

# target_link_libraries("$(llvm-config-10 --libs)") 



include_directories(${CMAKE_SOURCE_DIR}/include /usr/lib/llvm-10/include)
link_directories("/usr/lib/llvm-10/lib")
link_libraries("LLVM-10")
add_compile_options("${COMPILE_FLAGS}")

# 编译源码生成目标
add_executable(LAT init.cpp)

