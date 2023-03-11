#pragma once

#include "../parsing/CfgCtx.hpp"
#include "SocketHolder.h"
#include "../utils/SharedPtr.hpp"

#include <sys/select.h>
#include <vector>

namespace ft
{

class Server
{
public:
    void Run();
    Server(const std::vector<CfgCtx>& cfgCtxs);
private:
    void initReadWriteSets(fd_set &read, fd_set &write);
    std::vector<CfgCtx> m_configs;
    std::vector< Shared_ptr<SocketHolder> > m_listenSockets;
    std::vector< Shared_ptr<SocketHolder> > m_rwSockets;

    std::vector<VirtualServer> m_vServers;

    void initialize();
    ssize_t m_maxSelectFd;
};

} //namespace ft