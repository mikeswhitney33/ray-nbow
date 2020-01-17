#include "rtrandom.h"

#include <chrono>

RTRandom::RTRandom(float rmin, float rmax):distribution(rmin, rmax)
{
    auto beginning = std::chrono::high_resolution_clock::now();
    generator.seed(beginning.time_since_epoch().count());
}

float RTRandom::get()
{
    return distribution(generator);
}