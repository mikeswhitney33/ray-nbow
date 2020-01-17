#include <iostream>
#include "rtrandom.h"

int main(void)
{
    RTRandom rand(0, 1);
    std::cout << rand.get() << std::endl;
    return 0;
}