#include <iostream>
#include "network/SocketHolder.h"
#include "network/Server.hpp"
#include "parsing/CfgCtx.hpp"
#include "utils/SharedPtr.hpp"

#define LOCALHOST "127.0.0.1"

// namespace ft{

int main(int argc, char* argv[])
{
	if (argc == 1)
    	ft::Server("../webserv.conf").Run();
	else if (argc == 2)
		ft::Server(argv[1]).Run();
	else
		std::cerr << "Invalid arguments" << std::endl;

    return 0;
}
// } //namespace ft