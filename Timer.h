#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer {

    //Allows asynchronous code execution
    //without blocking the main thread
    std::thread Thread;
    bool Alive = false;
    //The "L" will keep the long type variable
    //Prevents conversion from long to int
    long CallNumber = -1L;
    long repeat_count = -1L;
    std::chrono::milliseconds interval = std::chrono::milliseconds(0);
    //"funct" accepts a function of type void
    //and has no parameters
    std::function<void(void)> funct = nullptr

    void SleepAndRun() {
        std::this_thread::sleep_for(interval);
        if(Alive) {
            //First set of parenthesis calls the function
            //itself. The second set of parenthesis calls
            //for the function that is returned by "Function"
            Function()();
        }
    }

    void ThreadFunc() {
        if(CallNumber == INFINITE) {
            while(Alive) {
                SleepAndRun();
            }
        }
        else {
            while(repeat_count--) {
                SleepAndRun();
            }
        }
    }
};

#endif // TIMER_H
