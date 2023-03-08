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

    std::string ConfigParser::get_server_block(const std::string& str)
    {
        size_t start, end;

        start = str.find("server {");
        if (start == std::string::npos)
            return "";

        start = str.find("{", start);

        end = str.find("server {", start);

        end = str.find_last_of('}', end);

        return str.substr(start + 1, end - start - 1);
    }

    std::string ConfigParser::get_location_block(const std::string& str)
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
        std::ifstream conf(filename, O_RDONLY);
        std::string tmp;

        std::getline(conf, tmp, '%');

        conf.close();

        tmp = clean_comments(tmp);

        std::string server = get_server_block(tmp);

        std::cout << server << std::endl;
    }
}

int main(int argc, char *argv[])
{
    ft::ConfigParser    cfg(argv[1]);

    return 0;
}