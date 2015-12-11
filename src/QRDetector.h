#ifndef QRDETECTOR_H
#define QRDETECTOR_H

#include <string>
class QRDetector
{
public:
    /**
     * /param url URL from which to grab an image.
     */
    QRDetector(std::string url);

    std::string detect();

protected:
    std::string url;
};

#endif //IMAGE_READER_SOURCE_H