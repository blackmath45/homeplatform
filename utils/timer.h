#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <ctime>
#include <time.h>

class timer
{
public:
    timer();

    double elapsed();

    void reset();

private:
    timespec beg_, end_;
};

#endif // TIMER_H