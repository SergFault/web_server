#include "Server.hpp"
#include "../server common.hpp"

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <sys/select.h>
#include "../parsers/ConfigParser.h"

namespace ft{

namespace {



} //namespace

void Server::initialize(){

    m_configs = ConfigParser().get_config(m_configsPath);

//    CfgCtx ctx;
//    ctx.ip = "127.0.0.1";
//    ctx.port = "8080";
//    m_configs.push_back(ctx);

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
        Shared_ptr<SocketHolder> socket_ptr;

        try
        {
            // Shared_ptr<SocketHolder> sh_p(new SocketHolder(AF_INET, SOCK_STREAM, 0));
            socket_ptr = Shared_ptr<SocketHolder>(new SocketHolder(AF_INET, SOCK_STREAM, 0));
        }
        catch(const std::exception &ex)
        {
            std::cerr << "Socket creation FAILED " << ex.what() << std::endl;
            throw std::exception();
        }

        std::cout << "There2" << std::endl;

        try
        {
            socket_ptr->bind(&server_address);
            socket_ptr->listen();
            socket_ptr->setNonBlocking();
        }
        catch(const std::exception &ex)
        {
            std::cerr << "Listener socket init FAILED " << ex.what() << std::endl;
            throw std::exception();
        }
        
        std::cout << "There2" << std::endl;

        m_listenSockets.push_back(socket_ptr);
    }
}

void Server::initReadWriteSets(fd_set &read, fd_set &write)
{
    for (std::vector< Shared_ptr<SocketHolder> >::iterator it = m_listenSockets.begin(); it != m_listenSockets.end(); it++)
    {
        FD_SET((*it)->getFd(), &read);

        if (m_maxSelectFd <= (*it)->getFd())
        {
            m_maxSelectFd = (*it)->getFd() + 1;
        }
    }

    for (std::vector< Shared_ptr<SocketHolder> >::iterator it = m_rwSockets.begin(); it != m_rwSockets.end(); it++)
    {
        FD_SET((*it)->getFd(), &write);
        FD_SET((*it)->getFd(), &read);

        if (m_maxSelectFd <= (*it)->getFd())
        {
            m_maxSelectFd = (*it)->getFd() + 1;
        }
    }
}


Server::Server(const std::string& confPath): m_configsPath(confPath), m_maxSelectFd(0)
{

}

void Server::Run()
{
    initialize();

    fd_set readFd;
    fd_set writeFd;


    for(int counter = 0; true ; )
    {
        FD_ZERO(&readFd);
        FD_ZERO(&writeFd);

        initReadWriteSets(readFd, writeFd);
        select(m_maxSelectFd, &readFd, &writeFd, NULL, NULL);

        /* accept all incoming connections and create read/write sockets */
        for (std::vector< Shared_ptr<SocketHolder> >::iterator it = m_listenSockets.begin(); it != m_listenSockets.end(); it++)
        {
            if ( FD_ISSET( (*it)->getFd(), &readFd) )
            {
                std::cout << "Listener FD mark YES-READ " << (*it)->getFd() << std::endl;
                Shared_ptr<SocketHolder> sh_h = (*it)->accept();
                if (sh_h->getFd() != -1)
                {
                    std::cout << "Accepted " << sh_h->getFd() << "Status" << sh_h->getStatus() << std::endl;
                    sh_h->setNonBlocking();
                    m_rwSockets.push_back(sh_h);
                }
            }
            std::cout << "Listener FD mark NO-READ " << (*it)->getFd() << std::endl;
        }

        /* READ REQ */
        for (std::vector< Shared_ptr<SocketHolder> > ::iterator it = m_rwSockets.begin(); it != m_rwSockets.end(); it++)
        {
            if ( FD_ISSET( (*it)->getFd(), &readFd) )
            {
                std::cout << "RW SOCKET mark YES-READ " << (*it)->getFd() << std::endl;
            
                (*it)->ProcessRead();            
            }
            std::cout << "RW SOCKET mark NO-READ " << (*it)->getFd() << std::endl;
        }

        /* WRITE RESP */
        for (std::vector< Shared_ptr<SocketHolder> >::iterator it = m_rwSockets.begin(); it != m_rwSockets.end(); it++)
        {
            if ( FD_ISSET( (*it)->getFd(), &writeFd) )
            {
                std::cout << "RW SOCKET mark: YES-WRITE " << (*it)->getFd() << std::endl;
            
                //todo send
                // std::cout << "REQ:" << std::endl << request_str << std::endl;
            
            }
            // it->sendFromRespHandler();
            std::cout << "RW SOCKET mark: NO-WRITE" << (*it)->getFd() << std::endl;
        }

        /* REMOVE DONE FD */
        for (std::vector< Shared_ptr<SocketHolder> >::iterator it = m_rwSockets.begin(); it != m_rwSockets.end();)
        {
            if ((*it)->getStatus() == Done)
            {
                std::cout << "socket: " << (*it)->getFd() << " is DONE! DELETING HIM!" << std::endl;

                // it++;
                std::cout << "ERASE iterator at " << (*it)->getFd() << std::endl;

                it = m_rwSockets.erase(it);

                // std::cout << "Now iterator at " << (*it)->getFd() << std::endl;
                // std::cout << "REQ:" << std::endl << request_str << std::endl;
            }
            else
            {
                it++;
            }
            // it->sendFromRespHandler();
        }

        std::cout << "<<<<<<<" << std::endl;
        usleep(2000000);
    }
}
}