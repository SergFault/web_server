#include "HttpReqHeader.hpp"

#include <string>

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

    HttpReqHeader::HttpReqHeader(const std::string& string)
    :
        m_method(""),
        m_uri(""),
        m_version(""),
        m_host("localhost"),
        m_port("80"),
        m_content_length(""),
        m_content_type(""),
        m_keep_alive(true),
        m_chunked(false),
        m_req_folder(false),
        m_rel_path(false)
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
        
        std::string headers = string.substr(headers_begin, string.size() - headers_begin);

        std::istringstream iline2(headers);
        
        while (std::getline(iline2, line))
        {
            std::istringstream ihead(line);

            // std::cout << "line: [" << line << "]" << std::endl; //debug;

            std::getline(ihead, line, ':');

            to_lower(line);

            // std::cout << "{" << line << "}" << std::endl; //debug

            if (line == "host")
            {
                std::getline(ihead, m_host, ':');
                m_host = trim_spaces(m_host);
                // std::cout << "host: [" << m_host << "]" << std::endl; //debug;
                std::getline(ihead, m_port);
                m_port = trim_spaces(m_port);
                // std::cout << "port: [" << m_port << "]" << std::endl; //debug
            }
            else if (line == "content-length")
            {
                std::getline(ihead, m_content_length, ':');
                m_content_length = trim_spaces(m_content_length);
                // std::cout << "content-length: [" << m_content_length << "]" << std::endl; //debug
            }
            else if (line == "content-type")
            {
                std::getline(ihead, m_content_type, ':');
                m_content_type = trim_spaces(m_content_type);
                // std::cout << "content-type: [" << m_content_type << "]" << std::endl; //debug
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

        // std::cout << "[" << headers << "]" << std::endl; //debug;

        parse_uri(m_uri);
    }

    int HttpReqHeader::parse_uri(const std::string& uri)
    {
        std::string tmp;
        std::string key, value;
        size_t pos;

        bool    abs_link = false;

        if ((pos = uri.find("//")) != std::string::npos)
        {
            m_st_uri.scheme = uri.substr(0, pos);
            abs_link = true;
            tmp = uri.substr(pos + 2, uri.size() - pos - 2);
        }
        else
        {
            tmp = uri;
        }
        
        if (m_st_uri.scheme != "http:" && m_st_uri.scheme != "")
            return -1; // unsupported scheme


        std::istringstream istr(tmp);
        
        std::getline(istr, tmp, '#');
        std::getline(istr, m_st_uri.fragment);

        std::istringstream istr2(tmp);

        std::getline(istr2, tmp, '?');

        while (std::getline(istr2, key, '='))
        {
            std::getline(istr2, value, '&');
            m_st_uri.query.insert(std::make_pair(key, value));
        }

        std::istringstream istr3(tmp);

        if (abs_link)
        {
            std::getline(istr3, tmp, '/');
            std::getline(istr3, m_st_uri.path);
            m_st_uri.path = "/" + m_st_uri.path;
        }
        else
        {
            std::getline(istr3, m_st_uri.path);
            if (m_st_uri.path[0] != '/')
                m_rel_path = true;
        }

        if (m_st_uri.path[m_st_uri.path.size() - 1] == '/')
            m_req_folder = true;
        
        if (abs_link)
        {   
            std::istringstream istr4(tmp);
            std::getline(istr4, m_st_uri.hostname, ':');
            std::getline(istr4, m_st_uri.port);
        }

        if (m_st_uri.hostname != "")
            m_host = m_st_uri.hostname;

        if (m_st_uri.port != "")
            m_port = m_st_uri.port;
        // std::cout << "scheme: [" << m_st_uri.scheme << "]" << std::endl; //debug
        std::cout << "hostname: [" << m_host << "]" << std::endl; //debug
        std::cout << "port: [" << m_port << "]" << std::endl; //debug
        // std::cout << "path: [" << m_st_uri.path << "]" << std::endl; //debug
        // for (auto it = m_st_uri.query.begin(); it != m_st_uri.query.end(); it++)
        //     std::cout << "query: [" << it->first << ":" << it->second << "]" << std::endl; //debug
        // std::cout << "fragment: [" << m_st_uri.fragment << "]" << std::endl; //debug
        
        return 0;
    }

    request_headers HttpReqHeader::get_req_headers()
    {
        struct request_headers res;

        res.query = m_st_uri.query;
        res.fragment = m_st_uri.fragment;
        res.path = m_st_uri.path;
        res.method = m_method;
        res.host = m_host;
        res.port = std::stoi(m_port);
        res.cont_length = std::stoi(m_content_length);
        res.keep_alive = m_keep_alive;
        res.chunked = m_chunked;
        res.content_type = m_content_type;
        res.req_folder = m_req_folder;
        res.rel_path = m_rel_path;

        return res;
    }
}

// int main()
// {
//     std::string str = "POST / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nContent-Length: 66\r\nContent-Type: multipart/form-data";

//     ft::HttpReqHeader req(str);

//     ft::request_headers res = req.get_req_headers();

//     std::cout << std::endl;

//     std::cout << res.st_uri.scheme << res.st_uri.hostname << res.st_uri.port
//         << res.st_uri.path << res.st_uri.fragment << std::endl;

//     std::cout << res.st_uri.path << std::endl;
//     std::cout << res.method << std::endl;
//     std::cout << res.host << std::endl;
//     std::cout << res.port << std::endl;
//     std::cout << res.cont_length << std::endl;
//     std::cout << res.content_type << std::endl;
//     std::cout << res.keep_alive << std::endl;
//     std::cout << res.chunked << std::endl;
//     std::cout << res.req_folder << std::endl;
//     std::cout << res.rel_path << std::endl;


//     return 0;
// }