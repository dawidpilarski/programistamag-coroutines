cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -stdlib=libc++ -fcoroutines-ts")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

#add_subdirectory(ThirdParty)
add_subdirectory(przykłady)