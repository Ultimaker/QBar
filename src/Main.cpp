#include <iostream>
#include <unistd.h>

#include "System/Clock.h"

#include "DBus/DBus.h"

int main(int argc, char** argv)
{
    Clock elapsed_time;

    while(true)
    {
        if( elapsed_time.getMilliseconds() < 50) //Force a FPS of about 25.
        {
            usleep(5 * 1000);
            continue;
        } else
        {
            elapsed_time.reset();
        }
        DBus::Bus::getInstance()->update();
    }
    return 0;
}