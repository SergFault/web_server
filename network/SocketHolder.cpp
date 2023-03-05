#include "SocketHolder.h"
#include <fcntl.h>

namespace ft
{


SocketHolder::SocketHolder(): m_file_descriptor(-1)
{

}

SocketHolder::SocketHolder(const SocketHolder &other)
{
    if (other.m_file_descriptor != -1)
    {
        m_obj_counter = other.m_obj_counter;
        ++(*m_obj_counter);
    }
    m_file_descriptor = other.m_file_descriptor;
}


SocketHolder& SocketHolder::operator=(const SocketHolder& other)
{
    if(&other == this)
    {
        return *this;
    }
    if (other.m_file_descriptor != -1)
    {
        m_obj_counter = other.m_obj_counter;
        ++(*m_obj_counter);
    }
    m_file_descriptor = other.m_file_descriptor;
    return *this;
}

SocketHolder::SocketHolder(int fd): m_file_descriptor(fd)
{
    if (fd != -1)
    {
        m_obj_counter = new int(0);
        ++(*m_obj_counter);
    }
}

int SocketHolder::getFd()
{
    return m_file_descriptor;
}

void SocketHolder::setNonBlocking()
{
    int flags = fcntl(m_file_descriptor, F_GETFL);
    if (flags < 0)
    {
        std::cout << "fd: "<< m_file_descriptor << std::endl;
        perror("falgs error: ");
        throw std::runtime_error("Can`t get fd flags \n");
    }
    if (fcntl(m_file_descriptor, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        throw std::runtime_error("Can`t set nonblocking option \n");
    }
}

SocketHolder::SocketHolder(int domain, int type, int protocol)
{
    m_file_descriptor = ::socket(domain, type,  protocol);

    if (m_file_descriptor == -1)
    {
        throw std::runtime_error("Creating socket: FAILED\n");
    }

     int opt = 1;
     if ((setsockopt(m_file_descriptor, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) == -1)
                   {
                        shutdown(m_file_descriptor, SHUT_RDWR);
                        close(m_file_descriptor);
                        throw std::runtime_error("Socket options application FAILED\n");
                   }

    m_obj_counter = new int(0);
    ++(*m_obj_counter);
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


    res =::read(m_file_descriptor, &buffer, 1023);

    // while ((res =::read(m_file_descriptor, &buffer, 1023)) == 1023)
    // {
    //     buffer[1024] = '\0';
    //     ss << buffer;
    // }
    // if (res == -1)
    // {
    //     perror("error:");
    //     throw std::runtime_error("Error while reading socket");
    // }

    if (res < 0)
    {
        return std::string();
    }

    buffer[res] = '\0';

    return std::string(buffer);
}

SocketHolder SocketHolder::accept()
{
    return SocketHolder(::accept(m_file_descriptor, NULL, NULL));
}

SocketHolder::~SocketHolder()
{
    // std::cout << "free " << m_file_descriptor << " objc " << *m_obj_counter <<  std::endl;
    if (m_file_descriptor != -1 && *m_obj_counter <= 1)
    {
        delete m_obj_counter;
        ::shutdown(m_file_descriptor, SHUT_RDWR);
        ::close(m_file_descriptor);
    }
}

} //namespace ft