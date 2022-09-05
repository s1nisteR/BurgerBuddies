cmake_minimum_required(VERSION 3.23)
project(BurgerBuddies)

include_directories(external/inifile-cpp/include)
set(CMAKE_CXX_STANDARD 20)
add_executable(BurgerBuddies src/main.cpp)

