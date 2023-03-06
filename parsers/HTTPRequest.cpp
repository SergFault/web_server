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

    void to_lower(std::string& str)
    {
        for (size_t i = 0; i < str.size(); i++)
            str[i] = std::tolower(str[i]);
    }

    HTTPRequest::HTTPRequest(const std::string& string)
    :
        m_method(""),
        m_uri(""),
        m_version(""),
        m_host("localhost"),
        m_port("80"),
        m_content_length(""),
        m_content_type(""),
        m_keep_alive(true),
        m_chunked(false)
    {
        std::istringstream ist(string); 
        std::string line;

        std::getline(ist, line);
        std::istringstream iline(line);

        std::getline(iline, m_method, ' ');
        std::cout << "[" << m_method << "]" << std::endl; //debug;
        
        std::getline(iline, m_uri, ' ');
        std::cout << "[" << m_uri << "]" << std::endl; //debug;
        
        std::getline(iline, m_version, '\r');
        std::cout << "[" << m_version << "]" << std::endl; //debug;

        size_t  headers_begin = string.find('\n') + 1;
        size_t  body = string.find("\r\n\r\n", 0);
        
        std::string headers = string.substr(headers_begin, body - headers_begin);

        std::istringstream iline2(headers);
        
        while (std::getline(iline2, line))
        {
            std::istringstream ihead(line);

            std::cout << "line: [" << line << "]" << std::endl; //debug;

            std::getline(ihead, line, ':');

            to_lower(line);

            std::cout << "{" << line << "}" << std::endl; //debug

            if (line == "host")
            {
                std::getline(ihead, m_host, ':');
                m_host = trim_spaces(m_host);
                std::cout << "host: [" << m_host << "]" << std::endl; //debug;
                std::getline(ihead, m_port);
                m_port = trim_spaces(m_port);
                std::cout << "port: [" << m_port << "]" << std::endl; //debug
            }
            else if (line == "content-length")
            {
                std::getline(ihead, m_content_length, ':');
                m_content_length = trim_spaces(m_content_length);
                std::cout << "content-length: [" << m_content_length << "]" << std::endl; //debug
            }
            else if (line == "content-type")
            {
                std::getline(ihead, m_content_type, ':');
                m_content_type = trim_spaces(m_content_type);
                std::cout << "content-type: [" << m_content_type << "]" << std::endl; //debug
            }
            else if (line == "connection")
            {
                std::string type;
                std::getline(ihead, type, ':');
                type = trim_spaces(type);
                if (type == "closed")
                    m_keep_alive = false;
            }
        }

        std::cout << "[" << headers << "]" << std::endl; //debug;
    }
}

int main()
{
    std::string str = "POST url HTTP/1.1\r\nHost: 192.168.0.1\r\nContent-Length: 66\r\nContent-Type: multipart/form-data\r\n\r\nbody";

    ft::HTTPRequest req(str);

    return 0;
}