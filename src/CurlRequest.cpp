#include "CurlRequest.h"
#include <curl/curl.h>

CurlRequest::CurlRequest(std::string url)
{
    curl = curl_easy_init();
    if(!curl)
    {
        throw std::string("Unable to initialize curl");
    }
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlRequest::curlWrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curl_buffer);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_perform(curl);
}

int CurlRequest::curlWrite(char* data, size_t size, size_t nmemb, std::string* buffer)
{
    int result = 0;
    if (buffer != nullptr)
    {
        buffer->append(data, size * nmemb);
        result = size * nmemb;
    }
    return result;
}

std::string CurlRequest::getData()
{
    return curl_buffer;
}
