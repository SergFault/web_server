#pragma once

#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <sstream>
/*
// default host: "localhost"
// default port: "80"
server {
    [listen: [host]:[port] [host]:[port];]
    [server_name: name1 name2;]

    root /path; # works for all locations without own root directive

    client_max_body_size (1m default, 0 unlimit);

    return [];

    error_page код ... [=[ответ]] uri;

    location {
        allowed_methods GET POST DELETE;
        autoindex on/off(default off);
        index file1 file2 file3; // предлагаю упростить до одного файла

        return [error_code] or [redirect_code redirect_uri];
    }
}

If a request ends with a slash, NGINX treats it as a request for a directory
and tries to find an index file in the directory. The index directive defines
the index file’s name (the default value is index.html). To continue with the
example, if the request URI is /images/some/path/, NGINX delivers the file
/www/data/images/some/path/index.html if it exists.
If it does not, NGINX returns HTTP code 404 (Not Found) by default.
To configure NGINX to return an automatically generated directory listing instead,
include the on parameter to the autoindex directive:

*/
namespace ft
{
    class ConfigParser
    {
    private:
        std::string get_server_block(const std::string& str);
        std::string clean_comments(const std::string& str);
        std::string get_location_block(const std::string& str);
    public:
        ConfigParser() {};
        explicit ConfigParser(const std::string& filename);
        ~ConfigParser() {};
    };
}