#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>
#include <iostream>

class Timer
{
public:
    void reset();
    friend std::ostream &operator<<(std::ostream &os, const Timer &timer);
private:
    std::chrono::high_resolution_clock::time_point start;
};

std::ostream &operator<<(std::ostream &os, const Timer &timer);

#endif // __TIMER_H__
