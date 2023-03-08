#pragma once

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <sstream>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 30

namespace ft
{

class SocketHolder
{
public:
    SocketHolder();
    explicit SocketHolder(int fd);
    SocketHolder(const SocketHolder& other);
    SocketHolder(int domain, int type, int protocol);

    ~SocketHolder();
    void bind(const struct sockaddr_in *addr);
    void listen();
    SocketHolder accept();
    void send(const std::string &);
    std::string read();
    SocketHolder& operator=(const SocketHolder& other);
    void setNonBlocking();
    int getFd();

private:
    // bool m_req_done = false;
    std::string m_req_string;
    int *m_obj_counter;
    int m_file_descriptor;

    sockaddr m_hostSockAdd;
    uint32_t m_hostSockAddrLen;
};

} // namespace ft