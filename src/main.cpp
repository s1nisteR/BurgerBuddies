#include <iostream>
#include <chrono>
#include <thread>

constexpr int RACK_SIZE = 10;               //size of the rack
constexpr int COOKING_TIME_MS = 1000;       //cooking time of each burger, in milliseconds

int rack = 0;

[[noreturn]] void cook()
{
    while(true)
    {
        //check if rack is full
        if(rack >= RACK_SIZE)
            continue; //if rack full, do nothing
        else
        {
            //otherwise, we COOK
            std::this_thread::sleep_for(std::chrono::milliseconds(COOKING_TIME_MS)); //cooking each burger takes 1 second
            rack++; //increment rack
        }
        std::cout << "Rack: " << rack << std::endl;
    }
}


int main()
{
    std::thread cook_thread(cook);
    cook_thread.join();
    std::cout << "Burger Buddies Problem" << std::endl;
    return 0;
}