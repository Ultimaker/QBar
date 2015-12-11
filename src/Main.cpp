#include <iostream>
#include <unistd.h>

#include "System/Clock.h"

#include "DBus/DBus.h"

#include "QRDetector.h"

int main(int argc, char** argv)
{
    Clock elapsed_time;
    //zxing::Ref<zxing::LuminanceSource> source;
    /*CurlRequest test("http://10.180.1.150:8080/?action=snapshot");


    int width, height;
    int size = 800 * 600 * 3;
    int comps = 0;
    zxing::ArrayRef<char> image;
    char *buffer = reinterpret_cast<char*>(jpgd::decompress_jpeg_image_from_memory(reinterpret_cast<const unsigned char *>(test.getData().c_str()), size, &width, &height, &comps, 4));
    image = zxing::ArrayRef<char>(buffer, 4 * width * height);
    free(buffer);
    zxing::Ref<zxing::LuminanceSource> source = zxing::Ref<zxing::LuminanceSource>(new ImageReaderSource(image, width, height, comps));

    int result = read_image(source, true);


    std::cout << result << std::endl;*/
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



