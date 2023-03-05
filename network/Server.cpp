#include "Server.hpp"

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

namespace ft{

void Server::initialize(){
    for (std::vector<CfgCtx>::iterator it = m_configs.begin(); it != m_configs.end(); it++)
    {
        struct sockaddr_in server_address;

        /* adress famaly */
        server_address.sin_family = AF_INET;

        /* string ip4 to binary and fill sockaddr_in */
        inet_pton(AF_INET, it->ip.c_str(), &(server_address.sin_addr));

        /* convert string port to binary and fill sockaddr_in */
        int port;
        std::stringstream(it->port) >> port;
        std::cout << "binding to port: " << port << std::endl;
        server_address.sin_port = htons(port);

        /* Socket creation */
        std::cout << "There" << std::endl;
        SocketHolder socket;

        try
        {
            socket = SocketHolder(AF_INET, SOCK_STREAM, 0);
        }
        catch(const std::exception &ex)
        {
            std::cerr << "Socket creation FAILED " << ex.what() << std::endl;
            throw std::exception();
        }

        std::cout << "There" << std::endl;

        try
        {
            socket.bind(&server_address);
            socket.listen();
            socket.setNonBlocking();
        }
        catch(const std::exception &ex)
        {
            std::cerr << "Listener socket init FAILED " << ex.what() << std::endl;
            throw std::exception();
        }
        
        m_sockets.push_back(socket);
    }
}

Server::Server(const std::vector<CfgCtx>& cfgCtxs): m_configs(cfgCtxs)
{

}

void Server::Run()
{
    initialize();

    std::vector<SocketHolder> readWriteSockets;
    for(int counter = 0; true ; )
    {

        /* accept all incoming connections and create read/write sockets */
        for (std::vector<SocketHolder>::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
        {
            SocketHolder sh = it->accept();
            if (sh.getFd() != -1)
            {
                std::cout << "Accepted " << sh.getFd() << std::endl;
                sh.setNonBlocking();
                readWriteSockets.push_back(sh);
            }
        }

        std::cout << "read sock size" << readWriteSockets.size() << std::endl;

        /* READ REQ */

        for (std::vector<SocketHolder>::iterator it = readWriteSockets.begin(); it != readWriteSockets.end(); it++)
        {
            std::string request_str = it->read();
            std::cout << "REQ:" << std::endl << request_str << std::endl;
        }

        std::cout << "<<<<<<<" << std::endl;
        usleep(1000000);


        // /* PREPARE RESP */
        // std::stringstream ss;
        // /* if "favicon ico" request - just ignore for now*/
        // if (request_str.find("favicon") != std::string::npos)
        // {
        //     ss << "HTTP/1.1 200 OK\r\n\r\n<html>\n\r<body>\n\r\r NO FAVICON \n\r</body>\n</html>";
        // } 
        // else
        // {
        //     ss << "HTTP/1.1 200 OK\r\n\r\n<html>\n\r<body>\n\r\rhello " << counter << " times\n\r</body>\n</html>";
        //     counter++;
        // }
        // accepted_socket.send(ss.str());
    }
    std::cout << "server run" << std::endl;
}


}