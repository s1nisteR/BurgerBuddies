cmake_minimum_required(VERSION 3.23)
project(BurgerBuddies)

add_subdirectory(external/FTXUI)
include_directories(external/FTXUI/include)
set(CMAKE_CXX_STANDARD 20)
add_executable(BurgerBuddies src/main.cpp)
target_link_libraries(BurgerBuddies
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
        )
