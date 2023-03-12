#include "ConfigParser.h"

namespace ft
{
    std::string ConfigParser::clean_comments(const std::string& str)
    {
        std::istringstream ist(str);
        std::string res;
        std::string tmp;

        while (std::getline(ist, tmp, '#'))
        {
            res += tmp;
            std::getline(ist, tmp);
        }
        
        return res;
    }

    std::string ConfigParser::get_server_block(std::string& str)
    {
        size_t start, end;

        start = str.find("server {");
        if (start == std::string::npos)
            return "";

        start = str.find("{", start);

        end = str.find("server {", start);

        end = str.find_last_of('}', end);

        std::string res = str.substr(start + 1, end - start - 1);

        str = str.substr(end, str.size() - end);

        return res;
    }

    std::string ConfigParser::get_location_block(std::string& str)
    {
        size_t  start, end, open_br, close_br;

        open_br = str.find("{");
        end = str.find("}");
        if (start == std::string::npos)
            return "";
        start = str.find("location ");
        start = str.find(' ', start);

    }

    ConfigParser::ConfigParser(const std::string& filename)
    {
        std::ifstream conf(filename, std::ios::in);
        std::string tmp;

        std::getline(conf, tmp, '%');

        conf.close();

        tmp = clean_comments(tmp);

        std::string server;

        while ((server = get_server_block(tmp)) != "")
        {
            CfgCtx  new_record;
            
            std::string location;
            
            while ((location = get_location_block(server)) != "")
            {

            }

            m_config.push_back(new_record);
        }
    }

    std::vector<CfgCtx> ConfigParser::get_config(const std::string& filename)
    {
        std::ifstream conf(filename, std::ios::in);
        std::string tmp;

        std::getline(conf, tmp, '%');

        conf.close();

        tmp = clean_comments(tmp);

        std::string server;

        while ((server = get_server_block(tmp)) != "")
        {

        }

        return m_config;
    }
}

// int main(int argc, char *argv[])
// {
//     ft::ConfigParser    cfg(argv[1]);

//     return 0;
// }