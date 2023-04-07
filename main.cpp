#include <iostream>
#include "network/Server.hpp"

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