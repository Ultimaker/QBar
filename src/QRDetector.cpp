#include "QRDetector.h"
#include "CurlRequest.h"

#include "Image/jpgd.h" //Required for decompress_jpeg_image_from_memory
#include "Image/ImageReaderSource.h"

#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/ReaderException.h>

#include <iostream>

QRDetector::QRDetector(std::string url): url(url)
{}

std::string QRDetector::detect()
{
    CurlRequest camera_request(url);

    int width = 800;
    int height = 600;
    int size = 800 * 600 * 3;
    int comps = 0;

    zxing::DecodeHints hints(zxing::DecodeHints::DEFAULT_HINT);
    zxing::Ref<zxing::Reader> reader(new zxing::MultiFormatReader);
    zxing::Ref<zxing::Result> result;

    // Decompress the jpeg from the obtained data.
    char* buffer = reinterpret_cast<char*>(jpgd::decompress_jpeg_image_from_memory(reinterpret_cast<const unsigned char *>(camera_request.getData().c_str()), size, &width, &height, &comps, 4));
    zxing::ArrayRef<char> image = zxing::ArrayRef<char>(buffer, 4 * width * height);
    free(buffer);

    // Convert the obtained data to the right format.
    zxing::Ref<zxing::LuminanceSource> source = zxing::Ref<zxing::LuminanceSource>(new ImageReaderSource(image, width, height, comps));
    zxing::Ref<zxing::Binarizer> binarizer;
    binarizer = new zxing::HybridBinarizer(source);
    zxing::Ref<zxing::BinaryBitmap> binary(new zxing::BinaryBitmap(binarizer));
    try
    {
        result = reader->decode(binary, hints);
    } catch (const zxing::ReaderException& e)
    {
        std::cout << "zxing::ReaderException: " + std::string(e.what()) << std::endl;
        return "";
    }

    return result->getText()->getText();
}

