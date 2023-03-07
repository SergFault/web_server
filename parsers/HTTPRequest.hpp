#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <map>

//==============================================================================//
//  ________  ______________    _____   ______  ___________________   ________  //
//  |scheme| |  hostname    |   |port|  |path|  |     query       |   |anchor|  //
//  |   ___| |______________|   |___|    |__|   |_________________|   |______|  //
//  |  |     |              |   |   |    |  |   |                 |   |      |  //
//  http :// www.hostname.com : port  /  path ? arg=val & arg2=val2 # fragment  //
//                                                                              //
//==============================================================================//

//==================================================//
//                                                  //
//      ANOTHER HOST WITH THE SAME SCHEME           //
//                                                  //
//      //<host> <path> [<query>] [<fragment>]      //
//                                                  //
//==================================================//

//==================================================//
//                                                  //
//      ABSOLUTE PATH WITHIN CURRENT HOST           //
//                                                  //
//      /<path> [<query>] [<fragment>]              //
//                                                  //
//==================================================//

//==================================================//
//                                                  //
//      RELATIVE PATH WITHIN CURRENT HOST           //
//                                                  //
//      <path> [<query>] [<fragment>]               //
//                                                  //
//==================================================//

//==================================================//
//                                                  //
//      ABSOLUTE PATH                               //
//                                                  //
//   <scheme> <host> [<path>] [<query>][<fragment>] //
//                                                  //
//==================================================//

namespace ft
{
    class HTTPRequest
    {
    private:

        struct uri
        {
            std::string scheme;
            std::string hostname;
            std::string port;
            std::string path;
            std::map<std::string, std::string>    query;
            std::string fragment;
        };

        struct uri  m_st_uri;
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
        ~HTTPRequest() {};

        int parse_uri(const std::string& uri);
    };
}