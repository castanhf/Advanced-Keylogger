#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer {

    // Allows asynchronous code execution
    // without blocking the main thread
    // Update compiler to TDC-GCC-64 because the default
    // compiler from MinGW32 does not recognize std::thread
    // Website to follow the setup of TDC-GCC-64 in CodeBlocks:
    // http://forums.codeblocks.org/index.php?topic=21570.0

    std::thread Thread;

    bool Alive = false; // to check if the timer is running

    // The "L" will keep the long type variable
    // Prevents conversion from long to int
    long CallNumber = -1L; // how many times we will need to call a certain function

    long repeat_count = -1L; // how many times a function has already been called

    std::chrono::milliseconds interval = std::chrono::milliseconds(0); // interval of time between function calls (default value is 0)

    // funct accepts a function of type void
    // and has no parameters
    // similar to a pointer to a function
    std::function<void(void)> funct = nullptr;


    void SleepAndRun() {

        std::this_thread::sleep_for(interval);
        if(Alive) {
            //First set of parenthesis calls "Function"
            //itself. The second set of parenthesis calls
            //for the function that is returned by "Function"
            Function()();
        }
    }

    // we have more control of this function because we let it pass pass it to a thread
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
    public:
        static const long Infinite = -1L;

        // default constructor
        Timer(){}

        // this constructor accepts a reference to a constant function object
        Timer(const std::function<void(void)> &f) : funct(f) {}

        // this constructor is has more parameters than the previous constructor
        Timer(const std::function<void(void)> &f,
              const unsigned long &i,
              const long repeat = Timer::Infinite): funct(f),
                                                    interval(std::chrono::milliseconds(i)),
                                                    CallNumber(repeat) {}

        // start the timer
        void Start(bool Async = true) {

            if(IsAlive()) {
                return;
            }
            Alive = true;
            repeat_count = CallNumber;
            if(Async) {
                Thread = std::thread(ThreadFunc, this);
            }
            else {
                this->ThreadFunc();
            }
        }

        // stop the timer
        void Stop() {

            Alive = false;
            Thread.join();
        }

        void setFunction(const std::function<void(void)> &f) {

            funct = f;
        }

        bool IsAlive() const {return Alive;}

        void RepeatCount(const long r) {

            if(Alive) {
                return;
            }
            CallNumber = r;
        }

        long GetLeftCount() const {return repeat_count;} // how many iterations are left

        long RepeatCount() const {return CallNumber;} // how many repetitions

        void SetInterval(const unsigned long &i) {

            if(Alive) {
                return;
            }
            interval = std::chrono::milliseconds(i);
        }

        unsigned long Interval() const {return interval.count();} // we use .count because interval is of type chrono::milliseconds

        const std::function<void(void)> &Function() const {
            return funct;
        }
};

#endif // TIMER_H
