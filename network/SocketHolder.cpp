#include "SocketHolder.h"
#include <arpa/inet.h>
#include <bitset>
#include <fcntl.h>

namespace ft
{


SocketHolder::SocketHolder(): m_file_descriptor(-1)
{
    m_obj_counter = new int(0);
    memset(&m_hostSockAdd, 0, sizeof(m_hostSockAdd));
}

SocketHolder::SocketHolder(const SocketHolder &other)
{
    memset(&m_hostSockAdd, 0, sizeof(m_hostSockAdd));
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

    if (m_file_descriptor == -1)
    {
        m_hostSockAdd = other.m_hostSockAdd;
        if (other.m_file_descriptor != -1)
        {
            m_obj_counter = other.m_obj_counter;
            ++(*m_obj_counter);
        }
        m_file_descriptor = other.m_file_descriptor;
    }
    else if (m_file_descriptor == 1)
    {
        std::cout << "SHITDOWN YES " << m_file_descriptor << std::endl;
        shutdown(m_file_descriptor, SHUT_RDWR);
        close(m_file_descriptor);
    }
    else
    {
        --(*m_obj_counter);
    }
    return *this;

}

SocketHolder::SocketHolder(int fd): m_file_descriptor(fd)
{
    memset(&m_hostSockAdd, 0, sizeof(m_hostSockAdd));
    if (fd != -1)
    {
        m_obj_counter = new int(1);
    }
    else
    {
        m_obj_counter = new int(0);
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
    memset(&m_hostSockAdd, 0, sizeof(m_hostSockAdd));

    if (m_file_descriptor == -1)
    {
        throw std::runtime_error("Creating socket: FAILED\n");
    }

     int opt = 1;
     if ((setsockopt(m_file_descriptor, SOL_SOCKET,
                   SO_REUSEADDR, &opt,
                   sizeof(opt))) == -1)
                   {
                        shutdown(m_file_descriptor, SHUT_RDWR);
                        close(m_file_descriptor);
                        throw std::runtime_error("Socket options application FAILED\n");
                   }

    m_obj_counter = new int(1);
}

bool SocketHolder::isWriterDone() const
{
    return (m_respWriter.getStatus() == Done);
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

void SocketHolder::sendFromRespHandler()
{
    int res;

    m_respWriter.readData(m_buffer);

    res =::send(m_file_descriptor, m_buffer.data, m_buffer.written, 0);

    if (res != m_buffer.written)
    {
        throw std::runtime_error("Error sending from response handler");
    }

    std::cout << "send: " << res << " bytes to fd: " << m_file_descriptor  << std::endl;
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
    int res = ::accept(m_file_descriptor, &m_hostSockAdd, &m_hostSockAddrLen);

    // std::cout << res << std::endl;

    SocketHolder sock(res);

    /* todo debug */
    if (sock.getFd() != -1)
    {
        std::cout << "NEW SOCKET ACCEPTED" << std::endl;
        std::cout << "  Host Address:" << inet_ntoa(reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)->sin_addr) << std::endl;
        std::cout << "  FD:" << sock.getFd() << std::endl;
        std::cout << "  Host Port:" << reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)->sin_port << std::endl;
    }
    // std::cout << reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)-> << std::endl;


    return sock;
}

SocketHolder::~SocketHolder()
{
    // std::cout << "free " << m_file_descriptor << " objc " << *m_obj_counter <<  std::endl;
    if (m_file_descriptor != -1 && *m_obj_counter <= 1)
    {
        std::cout << "FD" <<  m_file_descriptor << "SHUTDOWNN: YES. counter" << *m_obj_counter  << std::endl;
        delete m_obj_counter;
        ::shutdown(m_file_descriptor, SHUT_RDWR);
        ::close(m_file_descriptor);
    }
    else
    {
        std::cout << "FD" <<  m_file_descriptor << "SHUTDOWNN: NO. counter" << *m_obj_counter  << std::endl;
        --(*m_obj_counter);
    }
}

} //namespace ft