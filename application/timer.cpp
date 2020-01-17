#include "timer.h"


void Timer::reset()
{
    start = std::chrono::high_resolution_clock::now();
}

std::ostream &operator<<(std::ostream &os, const Timer &timer)
{
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - timer.start);
    return os << "Elapsed: " << duration.count() / 1e6 << "(sec)";
}