#pragma once

#include <fstream>
#include <iostream>
/*
// default host: "localhost"
// default port: "80"
server {
    [listen: [host]:[port] [host]:[port];]
    [server_name: name1 name2;]

    root [];

    client_max_body_size (1m default, 0 unlimit);

    

    error_page код ... [=[ответ]] uri;

    location {
        allowed_methods GET POST DELETE;
        autoindex on/off(default off);
        index file1 file2 file3; //(for requests with / at the end if autoindex off)

    }
}
*/
namespace ft
{
    class ConfigParser
    {
    private:
        std::ifstream _config_file;
    public:
        ConfigParser();
        ~ConfigParser();
    };
}