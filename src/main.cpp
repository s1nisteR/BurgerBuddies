#include <iostream>
#include <chrono>
#include <thread>
#include <semaphore>
#include <syncstream>
#include <inicpp.h>


int RACK_SIZE = 10;               //size of the rack
int COOKING_TIME_MS = 0;       //cooking time of each burger, in milliseconds
int NUMBER_OF_COOKS = 0;          //number of cooks
int NUMBER_OF_CASHIERS = 0;       //number of cashiers
int NUMBER_OF_CUSTOMERS = 0;     //number of customers

int rack = 0;

std::binary_semaphore rackMutex(1);
std::counting_semaphore fullCount(0);
std::counting_semaphore emptyCount(RACK_SIZE);
std::binary_semaphore customerMutex(1);
std::counting_semaphore customerSem(0);
std::counting_semaphore burger(0);


void assureState()
{
    if(rack < 0)
    {
        std::osyncstream(std::cout) << "Rack Underflow!" << std::endl;
    }
    else if(rack > RACK_SIZE)
    {
        std::osyncstream(std::cout) << "Rack Overflow!" << std::endl;
    }
    else
    {
        std::osyncstream(std::cout) << "Valid State" << std::endl;
    }
}

[[noreturn]] void cook(int id)
{
    while(true)
    {
        emptyCount.acquire(); //decrement cook sem


        //lock rack and put burger in it
        rackMutex.acquire();
        std::this_thread::sleep_for(std::chrono::milliseconds(COOKING_TIME_MS));
        assureState();
        rack++;
        assureState();
        rackMutex.release();

        std::osyncstream(std::cout) << "[COOK " << id << "] " << "Putting burger onto rack" << "\n";
        fullCount.release(); //increment cashier sem
    }
}

[[noreturn]] void cashier(int id)
{
    while(true)
    {
        customerSem.acquire(); //wait for customers
        fullCount.acquire();

        //lock rack and take burger out
        rackMutex.acquire();
        assureState();
        rack--;
        assureState();
        rackMutex.release();

        std::osyncstream(std::cout) << "[CASHIER " << id << "] " << "Taking burger from rack" << "\n";
        emptyCount.release();
        burger.release(); //signal that burger is given
    }
}


void customer(int id)
{
    customerMutex.acquire(); //enter CS for one customer at a time
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


    //joining all threads
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
