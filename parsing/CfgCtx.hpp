#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

namespace ft
{
    struct Location
    {
        std::string path;
        std::string root;
        std::string redirect_uri;
        bool    allow_get;
        bool    allow_post;
        bool    allow_del;
        bool    allow_cgi;
        bool    autoindex;
        u_short return_code;
        std::string index;
        bool    uploading;
        std::string upload_path;
    };

    struct VirtualServer
    {
        std::vector<Location>   locations;
        std::set<std::string> names;
        // добавить контейнер под пары host:port
        size_t  client_max_body_size;
        std::map<int, std::string> error_pages;//<код:путь> контейнер под error_pages
    };

    struct CfgCtx
    {
        CfgCtx(const std::string& ip, const std::string& port): ip(ip), port(port){};
        std::string ip;
        std::string port;
    };
} // ft