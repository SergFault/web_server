#pragma once

#include <iostream>
#include <string>
#include <sstream>

namespace ft
{
    class HTTPRequest
    {
    private:
        std::string _method;
        std::string _uri;
        std::string _version;
        std::string _host;
        std::string _port;
        std::string _content_length;
        bool        _chunked;
        
    public:
        HTTPRequest() {};
        HTTPRequest(const std::string& request);
    };
}