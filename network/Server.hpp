#pragma once

#include "../parsing/CfgCtx.hpp"
#include "SocketHolder.h"

#include <vector>

namespace ft{

class Server
{
public:
    void Run();
    Server(const std::vector<CfgCtx>& cfgCtxs);
private:
    std::vector<CfgCtx> m_configs;
    std::vector<SocketHolder> m_sockets;

    void initialize();

};
} //namespace ft