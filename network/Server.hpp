#pragma once

#include "../parsing/CfgCtx.hpp"
#include "SocketHolder.h"

#include <sys/select.h>
#include <vector>

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
    std::vector<std::string> names;
    // добавить контейнер под пары host:port
    size_t  client_max_body_size;
    // добавить контейнер под error_pages
};

class Server
{
public:
    void Run();
    Server(const std::vector<CfgCtx>& cfgCtxs);
private:
    void initReadWriteSets(fd_set &read, fd_set &write);
    std::vector<CfgCtx> m_configs;
    std::vector<SocketHolder> m_listenSockets;
    std::vector<SocketHolder> m_rwSockets;

    std::vector<VirtualServer> m_vServers;

    void initialize();
    ssize_t m_maxSelectFd;
};
} //namespace ft