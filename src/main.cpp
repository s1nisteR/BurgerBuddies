#include <iostream>
#include <chrono>
#include <thread>
#include <semaphore>
#include <syncstream>
#include <fstream>
#include <inicpp.h>


int RACK_SIZE = 0;               //size of the rack
int COOKING_TIME_MS = 0;       //cooking time of each burger, in milliseconds
int NUMBER_OF_COOKS = 0;          //number of cooks
int NUMBER_OF_CASHIERS = 0;       //number of cashiers
int NUMBER_OF_CUSTOMERS = 0;     //number of customers

int rack = 0;

std::binary_semaphore rackMutex(1);
std::counting_semaphore fullCount(0);
std::counting_semaphore emptyCount(RACK_SIZE); //TODO: Fix this. Try to initialize this semaphore later on.
std::binary_semaphore customerMutex(1);
std::counting_semaphore customerSem(0);
std::counting_semaphore burger(0);

[[noreturn]] void cook(int id)
{
    while(true)
    {
        emptyCount.acquire(); //decrement cook sem


        //lock rack and put burger in it
        rackMutex.acquire();
        std::this_thread::sleep_for(std::chrono::milliseconds(COOKING_TIME_MS));
        rack++;
        rackMutex.release();

        std::osyncstream(std::cout) << "[COOK " << id << "] " << "Putting burger onto rack" << "\n";
        fullCount.release(); //increment cashier sem
    }
}

[[noreturn]] void cashier(int id)
{
    //for debugging, cashier takes a burger out every 2s
    //in actual program replace with only when the customer wants
    while(true)
    {
        customerSem.acquire(); //wait for customers
        fullCount.acquire();

        //lock rack and take burger out
        rackMutex.acquire();
        rack--;
        rackMutex.release();

        std::osyncstream(std::cout) << "[CASHIER " << id << "] " << "Taking burger from rack" << "\n";
        emptyCount.release();
        burger.release(); //signal that burger is given
    }
}

void customer(int id)
{
    customerMutex.acquire(); //enter CS for one customer at a time
    sleep(2);
    //CS
    //signal cashier that a customer is present
    customerSem.release();
    burger.acquire();
    std::osyncstream(std::cout) << "[CUSTOMER " << id << "] " << "Received burger" << "\n";
    customerMutex.release(); //leave CS, allow other customers to enter CS and request for burgers

}


int main()
{
    //Check if config file exists
    std::ifstream myFile;
    myFile.open("config.ini");
    if(!myFile)
    {
        std::cout << "Config file does not exist!" << std::endl;
        return 0; //if it does not exist then exit
    }
    //otherwise load the config.ini file
    ini::IniFile configFile;
    configFile.decode(myFile);
    NUMBER_OF_COOKS = configFile["config"]["cooks"].as<int>();
    NUMBER_OF_CASHIERS = configFile["config"]["cashiers"].as<int>();
    NUMBER_OF_CUSTOMERS = configFile["config"]["customers"].as<int>();
    COOKING_TIME_MS = configFile["config"]["cooking_time_ms"].as<int>();
    RACK_SIZE = configFile["config"]["rack"].as<int>();


    std::cout << "=====Burger Buddies Problem=====" << std::endl;

    std::vector<std::thread> cooksArr;
    std::vector<std::thread> cashiersArr;
    std::vector<std::thread> customersArr;


    //Starting all cooks
    for(int i = 0; i < NUMBER_OF_COOKS; i++)
    {
        cooksArr.emplace_back(cook, i);
    }
    //Starting all cashiers
    for(int i = 0; i < NUMBER_OF_CASHIERS; i++)
    {
        cashiersArr.emplace_back(cashier, i);
    }
    //Starting all customers
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {
        customersArr.emplace_back(customer, i);
    }



    for(auto& x : cooksArr)
    {
        x.join();
    }
    for(auto& x : cashiersArr)
    {
        x.join();
    }
    for(auto& x : customersArr)
    {
        x.join();
    }

    return 0;
}
