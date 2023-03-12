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
            CfgCtx v_server;
            v_server.ip = "0.0.0.0";
            v_server.port = "80";

            //get locations
            
            std::istringstream  iss_server_params(server);
            std::string param;
            
            while (std::getline(iss_server_params, param, ';'))
            {
                std::istringstream  iss_key_values(param);
                std::string key;
                
                iss_key_values >> key;

                if (key == "client_max_body_size")
                {
                    std::string value;
                    iss_key_values >> value;
                    v_server.client_max_body_size = std::atoi(value.c_str());
                }
                else if (key == "listen")
                {
                    std::string tmp_ip = "";
                    std::string tmp_port = "";

                    iss_key_values >> std::ws;
                    std::getline(iss_key_values, tmp_ip, ':');
                    if (tmp_ip.find('.') == std::string::npos)
                    {
                        v_server.port = tmp_ip;
                    }
                    else
                    {
                        v_server.ip = tmp_ip;
                        iss_key_values >> tmp_port;
                        if (tmp_port != "")
                            v_server.port = tmp_port;
                    }
                }
                else if (key == "server_name")
                {
                    std::string s_name;
                    while (iss_key_values >> s_name)
                        v_server.server_names.insert(s_name);
                }
                else if (key == "error_page")
                {
                    u_short err_code;
                    std::string err_page;
                    iss_key_values >> err_code >> err_page;
                    v_server.error_pages.insert(std::make_pair(err_code, err_page));
                }
                else if (key == "root")
                {
                    iss_key_values >> v_server.root;
                }
            }

            m_config.push_back(v_server);
        }

        return m_config;
    }
}

int main(int argc, char *argv[])
{
    ft::ConfigParser    cfg;
    cfg.get_config(argv[1]);

    return 0;
}