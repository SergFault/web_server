#pragma once

#include <string>

struct CfgCtx
{
    CfgCtx(const std::string& ip, const std::string& port): ip(ip), port(port){};
    std::string ip;
    std::string port;
};