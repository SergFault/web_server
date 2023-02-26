#include "SocketHolder.h"

namespace ft
{


SocketHolder::SocketHolder(): m_file_descriptor(-1)
{

}

SocketHolder::SocketHolder(int fd): m_file_descriptor(fd)
{
}

SocketHolder::SocketHolder(int domain, int type, int protocol)
{
     m_file_descriptor = ::socket(domain, type,  protocol);

     int opt = 1;
     setsockopt(m_file_descriptor, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt));
}

void SocketHolder::bind(const struct sockaddr_in *addr)
{
     if (::bind(m_file_descriptor, reinterpret_cast<const struct sockaddr *>(addr), sizeof(*addr)) == -1)
     {
        std::cout << m_file_descriptor << std::endl;
        perror("error:\n");
        throw std::runtime_error("Error while binding\n");
     }
}

void SocketHolder::listen()
{
    if (::listen(m_file_descriptor, BACKLOG) == -1)
    {
        perror("error:");
        throw std::runtime_error("Error while listen");
    }
}

void SocketHolder::send(const std::string & str)
{
    int res;
    if ((res =::send(m_file_descriptor, str.c_str(), str.size(), 0)) == -1)
    {
        perror("error:");
        throw std::runtime_error("Error while listen");
    }
    std::cout << "send: " << res << std::endl;
}

std::string SocketHolder::read()
{
    int res;
    char buffer[1024];

    std::stringstream ss;

    while ((res =::read(m_file_descriptor, &buffer, 1023)) == 1023)
    {
        buffer[1024] = '\0';
        ss << buffer;
    }
    if (res == -1)
    {
        perror("error:");
        throw std::runtime_error("Error while reading socket");
    }
    buffer[res] = '\0';
    ss << buffer;
    return ss.str();
}

int SocketHolder::accept()
{
    int fd;

    if ((fd = ::accept(m_file_descriptor, NULL, NULL)) == -1)
    {
        perror("error:");
        throw std::runtime_error("Error while accepting");
    }

    return fd;
}

SocketHolder::~SocketHolder()
{
    if (m_file_descriptor != -1)
    {
        ::shutdown(m_file_descriptor, SHUT_RDWR);
        ::close(m_file_descriptor);
    }
    std::cout << "socket destroyed" << std::endl;
}

int SocketHolder::moveFd()
{
    int fd = m_file_descriptor;
    m_file_descriptor = -1;
    return fd;
}

} //namespace ft