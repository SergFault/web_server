#include "HTTPRequest.hpp"

namespace ft
{

    std::string trim_spaces(const std::string& str)
    {
        size_t begin, end;

        begin = str.find_first_not_of(" \r\n\t");
        if (begin == std::string::npos)
            begin = 0;
        end = str.find_first_of(" \r\n\t", begin);
        if (end == std::string::npos)
            end = str.size();

        return str.substr(begin, end - begin);
    }

    HTTPRequest::HTTPRequest(const std::string& string)
    {
        std::istringstream ist(string); 
        std::string line;
        // size_t  pos1, pos2;

        std::getline(ist, line);
        std::istringstream iline(line);

        std::getline(iline, _method, ' ');
        std::cout << "[" << _method << "]" << std::endl; //debug;
        
        std::getline(iline, _uri, ' ');
        std::cout << "[" << _uri << "]" << std::endl; //debug;
        
        std::getline(iline, _version, '\r');
        std::cout << "[" << _version << "]" << std::endl; //debug;

        size_t  headers_begin = string.find('\n') + 1;
        size_t  body = string.find("\r\n\r\n", 0);
        
        std::string headers = string.substr(headers_begin, body - headers_begin);

        std::istringstream iline2(headers);
        
        while (std::getline(iline2, line))
        {
            std::istringstream ihead(line);

            std::cout << "line: [" << line << "]" << std::endl; //debug;

            std::getline(ihead, line, ':');

            if (line == "Host")
            {
                std::getline(ihead, _host, ':');
                _host = trim_spaces(_host);
                std::cout << "host: [" << _host << "]" << std::endl; //debug;
                std::getline(ihead, _port);
                _port = trim_spaces(_port);
                std::cout << "port: [" << _port << "]" << std::endl; //debug
            }
        }

        std::cout << "[" << headers << "]" << std::endl; //debug;

        // pos1 = string.find(" ", 0);
        // _method = string.substr(0, pos1);
        // std::cout << "[" << _method << "]" << std::endl; //debug;
        // pos1 = string.find_first_not_of(" ", pos1);
        // pos2 = string.find(" ", pos1);
        // _uri = string.substr(pos1, pos2 - pos1);
        // pos1 = string.find_first_not_of(" ", pos2);
        // pos2 = string.find("\r\n", pos1);
        // _version = string.substr(pos1, pos2 - pos1);
        // std::cout << "[" << _uri << "]" << std::endl; //debug;
        // std::cout << "[" << _version << "]" << std::endl; //debug;

        // pos1 = string.find("Host: ", pos2);
        // if (pos1 == std::string::npos)
        // {
        //     _host = "localhost";
        //     _port = "80";
        // }
    }
}

int main()
{
    std::string str = "POST url HTTP/1.1\r\nHost: 192.168.0.1\r\nContent-Length:66\r\n\r\nbody";

    ft::HTTPRequest req(str);

    return 0;
}