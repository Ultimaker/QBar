#ifndef CURL_REQUEST_H
#define CURL_REQUEST_H

#include <string>

// CURL is a c lib, so this is the 'forward declaration'
typedef void CURL;

class CurlRequest
{
public:
    CurlRequest(std::string url);
    static int curlWrite(char* data, size_t size, size_t nmemb, std::string* buffer);
    std::string getData();
protected:
    CURL * curl;
    std::string curl_buffer;
};

#endif //CURL_REQUEST_H