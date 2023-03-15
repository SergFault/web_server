#pragma once

#include "../server common.hpp"
#include "../utils/SharedPtr.hpp"
#include "../body_handler/IBodyHandler.hpp"
#include "../body_handler/UploadBodyHandler.hpp"
#include "../process/Handler.hpp"

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


namespace ft{

enum ProcessStatus
{
    ReadRequest,
    ReadBody,
    ReadDone,
    WriteBody,
    WriteRequest,
    Done,
};

class SocketHolder
{
public:
    SocketHolder(int domain, int type, int protocol);
    SocketHolder(int desc);

    ~SocketHolder();
    void bind(const struct sockaddr_in *addr);
    void listen();
    Shared_ptr<SocketHolder> accept();
    void send(const std::string&);
    std::string read();
    ProcessStatus getStatus() const;
    void setNonBlocking();
    int getFd();
    void ProcessRead();
    void ProcessWrite();


private:
    SocketHolder();
    void SetNextState();
    void AccumulateRequest(const std::string& reqChunk);
    void HandleBody(std::string& str);
    // bool m_req_done = false;

    /* whole request as string */
    std::string m_req_string;
    std::string m_remainAfterRequest;

    void InitBodyHandler();
    void InitWriteHandler();

    // Shared_ptr<IInputHandler> m_bodyHandler;
    Shared_ptr<IOutputHandler> m_writeHandler;


    int m_file_descriptor;

    /* host info */
    sockaddr m_hostSockAdd;
    uint32_t m_hostSockAddrLen;

    ProcessStatus m_procStatus;

    char m_buffer[1024];
};


} // namespace ft