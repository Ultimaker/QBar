#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

// getMilliseconds returns an monotonic time (uneffected by clock changes) in milliseconds.
uint64_t getMilliseconds();

/**
    The Clock class helps in keeping track of time. It acts as a simple stopwatch which can be reset to restart the counter.
*/
class Clock
{
private:
    uint64_t start_time;
public:
    Clock();
    
    void reset();
    
    uint64_t getMilliseconds();
};

#endif//CLOCK_H
