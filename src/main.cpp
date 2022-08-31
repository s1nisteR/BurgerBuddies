#include <iostream>
#include <chrono>
#include <thread>
#include <semaphore>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

constexpr int RACK_SIZE = 10;               //size of the rack
constexpr int COOKING_TIME_MS = 2000;       //cooking time of each burger, in milliseconds

int rack = 0;
std::binary_semaphore sem(1);

[[noreturn]] void cook()
{
    while(true)
    {
        //check if rack is full
        if(rack >= RACK_SIZE)
        {
            continue; //if rack full, do nothing
        }
        else
        {
            //otherwise, we COOK
            std::this_thread::sleep_for(std::chrono::milliseconds(COOKING_TIME_MS)); //cooking each burger takes 1 second
            sem.acquire();
            rack++; //increment rack
            sem.release();
            //std::cout << "Produced: Rack=" << rack << std::endl;
        }
    }
}

[[noreturn]] void cashier()
{
    //for debugging, cashier takes a burger out every 2s
    //in actual program replace with only when the customer wants
    while(true)
    {
        if(rack <= 0)
        {
            continue;
        }
        else
        {
            //otherwise we take one from the rack and serve to the customer
            sem.acquire();
            rack--; //decrement rack
            sem.release();
            //std::cout << "Consumed: Rack=" << rack << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}

int main()
{
    std::thread cook_thread(cook);
    std::thread cashier_thread(cashier);
    using namespace ftxui;

    // Define the document
    Element document =
            hbox({
                text("CSE325(2)") | border,
                center(text("Burger Buddies Problem")) | border | flex,
                text("Group 2") | border,
                });

    auto screen = Screen::Create(
            Dimension::Full(),       // Width
            Dimension::Fit(document) // Height
    );
    Render(screen, document);
    screen.Print();
    cook_thread.join();
    cashier_thread.join();
    //std::cout << "Burger Buddies Problem" << std::endl;
    return 0;
}
