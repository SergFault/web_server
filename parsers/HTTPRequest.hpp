#pragma once

#include <iostream>
#include <string>
#include <sstream>

namespace ft
{
    class HTTPRequest
    {
    private:
        std::string m_method;
        std::string m_uri;
        std::string m_version;

        std::string m_host;
        std::string m_port;
        
        std::string m_content_length;
        
        std::string m_content_type;
        
        bool        m_keep_alive;
        bool        m_chunked;
        
    public:
        HTTPRequest() {};
        HTTPRequest(const std::string& request);

        int parse_uri(const std::string& uri) {};
    };
}