#ifndef __RTRANDOM_H__
#define __RTRANDOM_H__

#include <random>
#include <chrono>

class RTRandom
{
public:
    RTRandom(float rmin, float rmax);
    float get();
private:
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
};

#endif // __RTRANDOM_H__