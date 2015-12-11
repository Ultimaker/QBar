#include <iostream>
#include <unistd.h>

#include "System/Clock.h"

#include "DBus/DBus.h"

#include "Image/ImageReaderSource.h"

#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/ReaderException.h>

#include "CurlRequest.h"
#include "Image/jpgd.h"


std::vector<zxing::Ref<zxing::Result> > decode(zxing::Ref<zxing::BinaryBitmap> image, zxing::DecodeHints hints)
{
    zxing::Ref<zxing::Reader> reader(new zxing::MultiFormatReader);
    return std::vector<zxing::Ref<zxing::Result> >(1, reader->decode(image, hints));
}

int read_image(zxing::Ref<zxing::LuminanceSource> source, bool hybrid) {
    std::vector<zxing::Ref<zxing::Result> > results;
    std::string cell_result;
    int result_code = -1;

    try
    {
        zxing::Ref<zxing::Binarizer> binarizer;
        if (hybrid)
        {
            binarizer = new zxing::HybridBinarizer(source);
        } else
        {
            binarizer = new zxing::GlobalHistogramBinarizer(source);
        }
        zxing::DecodeHints hints(zxing::DecodeHints::DEFAULT_HINT);
        zxing::Ref<zxing::BinaryBitmap> binary(new zxing::BinaryBitmap(binarizer));

        results = decode(binary, hints);

        result_code = 0;
    } catch (const zxing::ReaderException& e)
    {
        cell_result = "zxing::ReaderException: " + std::string(e.what());
        result_code = -2;
    } catch (const zxing::IllegalArgumentException& e)
    {
        cell_result = "zxing::IllegalArgumentException: " + std::string(e.what());
        result_code = -3;
    } catch (const zxing::Exception& e)
    {
        cell_result = "zxing::Exception: " + std::string(e.what());
        result_code = -4;
    } catch (const std::exception& e)
    {
        cell_result = "std::exception: " + std::string(e.what());
        result_code = -5;
    }
    std::cout << cell_result << std::endl;

    for (size_t i = 0; i < results.size(); i++)
    {
        std::cout << results[i]->getText()->getText() << std::endl;
    }
    return result_code;
}

int main(int argc, char** argv)
{
    Clock elapsed_time;
    //zxing::Ref<zxing::LuminanceSource> source;
    CurlRequest test("http://10.180.1.150:8080/?action=snapshot");


    int width, height;
    int size = 800 * 600 * 3;
    int comps = 0;
    zxing::ArrayRef<char> image;
    char *buffer = reinterpret_cast<char*>(jpgd::decompress_jpeg_image_from_memory(reinterpret_cast<const unsigned char *>(test.getData().c_str()), size, &width, &height, &comps, 4));
    image = zxing::ArrayRef<char>(buffer, 4 * width * height);
    free(buffer);
    zxing::Ref<zxing::LuminanceSource> source = zxing::Ref<zxing::LuminanceSource>(new ImageReaderSource(image, width, height, comps));

    int result = read_image(source, true);


    std::cout << result << std::endl;
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



