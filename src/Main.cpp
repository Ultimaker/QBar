#include <iostream>
#include <unistd.h>

#include "System/Clock.h"

#include "DBus/DBus.h"

#include "QRDetector.h"

int main(int argc, char** argv)
{
    Clock elapsed_time;
    QRDetector detector("http://10.180.1.150:8080/?action=snapshot");

    while(true)
    {
        if( elapsed_time.getMilliseconds() < 500)
        {
            usleep(5 * 1000);
            continue;
        } else
        {
            elapsed_time.reset();
        }
        std::cout << detector.detect() << std::endl;
        //DBus::Bus::getInstance()->update();
    }
    return 0;
}



