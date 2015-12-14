#ifndef CURL_REQUEST_H
#define CURL_REQUEST_H

#include <string>

// CURL is a c lib, so this is the 'forward declaration'
typedef void CURL;

class CurlRequest
{
public:
    /** The Curl Request object is a single call to an URL.
     *  If you want to make multiple calls, the object should be recreated.
     *  Overhead wise this is not the best way to do it, but it shouldn't be that much slower.
     */
    CurlRequest(std::string url);

    /** Function required for the libcurl to write data to the buffer
     */
    static int curlWrite(char* data, size_t size, size_t nmemb, std::string* buffer);

    /** Get the data from the request. 
     */
    std::string getData();
protected:
    CURL * curl;
    std::string curl_buffer;
};

#endif //CURL_REQUEST_H