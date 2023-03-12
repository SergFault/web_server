#include "ConfigParser.h"

#include <cstdlib>

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
        size_t  start, end, open_br, path_start;
        std::string location;

        open_br = str.find("{");
        end = str.find("}");
        if (open_br == std::string::npos)
            return "";
        start = str.rfind("location ", open_br);
        path_start = str.find_first_not_of(' ', start + 8);

        location = str.substr(path_start, end - path_start);
        str = str.substr(0, start) + str.substr(end + 1, str.size() - end - 1);
        
        open_br = location.find('{');
        location[open_br] = ' ';
        
        return location;
    }

    ConfigParser::ConfigParser(const std::string& filename)
    {
        // std::ifstream conf(filename, std::ios::in);
        // std::string tmp;

        // std::getline(conf, tmp, '%');

        // conf.close();

        // tmp = clean_comments(tmp);

        // std::string server;

        // while ((server = get_server_block(tmp)) != "")
        // {
        //     CfgCtx  new_record;
            
        //     std::string location;
            
        //     while ((location = get_location_block(server)) != "")
        //     {

        //     }

        //     m_config.push_back(new_record);
        // }
    }

    std::vector<CfgCtx> ConfigParser::get_config(const std::string& filename)
    {
        std::ifstream conf(filename.c_str(), std::ios::in);
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

            std::string location;
            while ((location = get_location_block(server)) != "")
            {
                Location    new_location;

                std::istringstream  iss_location(location);
                
                iss_location >> new_location.path;
                std::string l_params;
                while (std::getline(iss_location, l_params, ';'))
                {
                    std::istringstream  iss_loc_values(l_params);
                    std::string l_arg;

                    iss_loc_values >> l_arg;

                    if (l_arg == "root")
                    {
                        iss_loc_values >> new_location.root;
                    }
                    else if (l_arg == "return")
                    {
                        iss_loc_values >> new_location.return_code;
                        iss_loc_values >> new_location.redirect_uri;
                    }
                    else if (l_arg == "accepted_methods")
                    {
                        std::string method;
                        while (iss_loc_values >> method)
                        {
                            if (method == "GET")
                                new_location.allow_get = true;
                            else if (method == "POST")
                                new_location.allow_post = true;
                            else if (method == "DELETE")
                                new_location.allow_del = true;
                        }
                    }
                    else if (l_arg == "autoindex")
                    {
                        std::string allow;
                        iss_loc_values >> allow;
                        if (allow == "on")
                            new_location.autoindex = true;
                    }
                    else if (l_arg == "upload_path")
                    {
                        new_location.uploading = true;
                        iss_loc_values >> new_location.upload_path;
                    }
                    else if (l_arg == "cgi")
                    {
                        new_location.allow_cgi = true;
                    }
                }

                v_server.location_paths.insert(new_location.path);
                v_server.locations.insert(std::make_pair(new_location.path, new_location));
            }
            //get locations
            
            std::istringstream  iss_server_params(server);
            std::string param;
            
            while (std::getline(iss_server_params, param, ';'))
            {
                // if (key == "client_max_body_size")
                std::istringstream  iss_key_values(param);
                std::string key;
                
                iss_key_values >> key;

                if (key == "client_max_body_size")
                {
                    std::string value;
                    iss_key_values >> value;
                    v_server.client_max_body_size = atoi(value.c_str());
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

            for (std::map<std::string, Location>::iterator it = v_server.locations.begin();
                    it != v_server.locations.end(); ++it)
            {
                if ((*it).second.root == "")
                    (*it).second.root = v_server.root;
            }
            m_config.push_back(v_server);
        }

        return m_config;
    }
}

// int main(int argc, char *argv[])
// {
//     ft::ConfigParser    cfg;
//     std::vector<ft::CfgCtx> vec = cfg.get_config(argv[1]);
//     std::vector<ft::CfgCtx>::iterator it1;

//     int i = 0;

//     for (it1 = vec.begin(); it1 != vec.end(); ++it1)
//     {
//         std::cout << "{SERVER [" << i << "]}:" << std::endl;
//         i++;
//         std::cout << (*it1).ip << ":" << (*it1).port << std::endl;
//         for (auto it2 = (*it1).locations.begin(); it2 != (*it1).locations.end(); ++it2)
//         {
//             std::cout << "location: " << (*it2).first << std::endl;
//             std::cout << (*it2).second.root << std::endl;
//         }
//     }

//     return 0;
// }