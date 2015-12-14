#ifndef QRDETECTOR_H
#define QRDETECTOR_H

#include <string>
class QRDetector
{
public:
    /** Simple QR detector that grabs an image from provided URL.
     * /param url URL from which to grab an image.
     */
    QRDetector(std::string url);

    /** Detect grabs the frame from the URL and returns the data in the detected QR code (if any)
     * /returns string containing the detected data. The string is empty if no QR code is detected.
     */
    std::string detect();

protected:
    std::string url;
};

#endif //IMAGE_READER_SOURCE_H