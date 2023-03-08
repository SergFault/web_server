#include "Server.hpp"

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <sys/select.h>

namespace ft{

namespace {



} //namespace

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
        
        m_listenSockets.push_back(socket);
    }
}

void Server::initReadWriteSets(fd_set &read, fd_set &write)
{
    for (std::vector<SocketHolder>::iterator it = m_listenSockets.begin(); it != m_listenSockets.end(); it++)
    {
        FD_SET(it->getFd(), &read);
        FD_SET(it->getFd(), &read);
        if (m_maxSelectFd <= it->getFd())
        {
            m_maxSelectFd = it->getFd();
        }
    }
    for (std::vector<SocketHolder>::iterator it = m_rwSockets.begin(); it != m_rwSockets.end(); it++)
    {
        FD_SET(it->getFd(), &read);
        FD_SET(it->getFd(), &read);
        {
            m_maxSelectFd = it->getFd();
        }
    }
}


Server::Server(const std::vector<CfgCtx>& cfgCtxs): m_configs(cfgCtxs), m_maxSelectFd(0)
{

}

void Server::Run()
{
    initialize();

    fd_set readFd;
    fd_set writeFd;

    FD_ZERO(&readFd);
    FD_ZERO(&writeFd);

    for(int counter = 0; true ; )
    {

        initReadWriteSets(readFd, writeFd);
        select(m_maxSelectFd, &readFd, &writeFd, NULL, NULL);

        /* accept all incoming connections and create read/write sockets */
        for (std::vector<SocketHolder>::iterator it = m_listenSockets.begin(); it != m_listenSockets.end(); it++)
        {
            if ( FD_ISSET( it->getFd(), &readFd) )
            {
                std::cout << "Listener FD mark for READ " << it->getFd() << std::endl;
                SocketHolder sh = it->accept();
                if (sh.getFd() != -1)
                {
                    std::cout << "Accepted " << sh.getFd() << std::endl;
                    sh.setNonBlocking();
                    m_rwSockets.push_back(sh);
                }
            }
            std::cout << "Listener FD NOT mark for READ " << it->getFd() << std::endl;
        }

        /* READ REQ */
        for (std::vector<SocketHolder>::iterator it = m_rwSockets.begin(); it != m_rwSockets.end(); it++)
        {
            if ( FD_ISSET( it->getFd(), &readFd) )
            {
                std::cout << "RW SOCKET mark for READ " << it->getFd() << std::endl;
            
                std::string request_str = it->read();
                std::cout << "REQ:" << std::endl << request_str << std::endl;
            
            }
            std::cout << "RW SOCKET NOT mark for READ " << it->getFd() << std::endl;
        }

        std::cout << "<<<<<<<" << std::endl;
        usleep(3000000);


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