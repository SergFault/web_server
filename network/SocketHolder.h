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
    SocketHolder(int domain, int type, int protocol);
    ~SocketHolder();
    void bind(const struct sockaddr_in *addr);
    void listen();
    int accept();
    void send(const std::string &);
    std::string read();
    explicit SocketHolder(int fd);
    int moveFd();


private:
    explicit SocketHolder(SocketHolder &other);
    SocketHolder &operator=(const SocketHolder &other);

    int m_file_descriptor;
};

} // namespace ft