#include <iostream>
#include <chrono>
#include <thread>
#include <semaphore>


constexpr int RACK_SIZE = 10;               //size of the rack
constexpr int COOKING_TIME_MS = 500;       //cooking time of each burger, in milliseconds

int rack = 0;

std::binary_semaphore rackMutex(1);
std::counting_semaphore cashierSemaphore(0);
std::counting_semaphore cookSemaphore(RACK_SIZE);

[[noreturn]] void cook()
{
    while(true)
    {
        cookSemaphore.acquire(); //decrement cook sem

        std::this_thread::sleep_for(std::chrono::milliseconds(COOKING_TIME_MS));

        //lock rack and put burger in it
        rackMutex.acquire();
        rack++;
        rackMutex.release();

        std::cout << "Produced: Rack=" << rack << std::endl;
        cashierSemaphore.release(); //increment cashier sem
    }
}

[[noreturn]] void cashier()
{
    //for debugging, cashier takes a burger out every 2s
    //in actual program replace with only when the customer wants
    while(true)
    {
        sleep(3);
        cashierSemaphore.acquire(); //decrement cashier sem

        //lock rack and take burger out
        rackMutex.acquire();
        rack--;
        rackMutex.release();

        std::cout << "Consumed: Rack=" << rack << std::endl;
        cookSemaphore.release(); //increment cook sem
    }
}



int main()
{
    std::thread cook_thread(cook);
    std::thread cashier_thread1(cashier);
    std::thread cashier_thread2(cashier);


    cook_thread.join();
    cashier_thread1.join();
    cashier_thread2.join();

    std::cout << "Burger Buddies Problem" << std::endl;
    return 0;
}
