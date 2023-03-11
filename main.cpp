#include <iostream>
#include "network/SocketHolder.h"
#include "network/Server.hpp"
#include "parsing/CfgCtx.hpp"
#include "utils/SharedPtr.hpp"

#include <vector>


#define LOCALHOST "127.0.0.1"

// namespace ft{

int main(){

    /* todo make proper config parsing */
    std::vector<CfgCtx> contexts;
    contexts.push_back(CfgCtx("127.0.0.1", "8080"));
    contexts.push_back(CfgCtx("127.0.0.1", "8081"));
    contexts.push_back(CfgCtx("127.0.0.1", "8082"));



    ft::Server(contexts).Run();

    return 0;
}
// } //namespace ft