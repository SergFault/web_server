#pragma once

#include "../server common.hpp"
#include "../utils/SharedPtr.hpp"
#include "../body_handler/IBodyHandler.hpp"
#include "../body_handler/UploadBodyHandler.hpp"
#include "../process/Handler.hpp"
#include "../parsers/HttpReqHeader.hpp"
#include "../parsing/CfgCtx.hpp"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 30


namespace ft{

enum ProcessStatus
{
    ReadRequest,
    ReadBody,
    WriteRequest,
    Done
};

class SocketHolder
{
public:
    SocketHolder(int domain, int type, int protocol, const std::vector<CfgCtx>& ctxs);
    SocketHolder(int desc, const std::vector<CfgCtx>& ctxs);

    ~SocketHolder();
    void bind(struct sockaddr_in *addr);
    void listen();
    Shared_ptr<SocketHolder> accept();
    void send(const std::string&);
    std::string read();
    ProcessStatus getStatus() const;
    void setNonBlocking();
    int getFd();
    void ProcessRead();
    void ProcessWrite();
    const std::string& getServerIp() const;
    const std::string& getServerPort() const;

private:
    SocketHolder();
    // void SetNextState();
    void AccumulateRequest();
    void HandleBody(void);
    // bool m_req_done = false;

    /* whole request as string */
    std::string m_req_string;
    std::string m_remainAfterRequest;

    /* parsed req header*/
    Shared_ptr<HttpReqHeader> m_reqHeader;

    void InitBodyHandler();
    void InitWriteHandler();
	void SetVServer();
	void SetLocation();

     Shared_ptr<IInputHandler> m_bodyHandler;
    Shared_ptr<IOutputHandler> m_writeHandler;


    int m_file_descriptor;

    /* host info */
    sockaddr m_hostSockAdd;
    uint32_t m_hostSockAddrLen;

	std::string m_serverIp;
public:
	void SetMServerIp(const std::string &m_server_ip);
	void SetMServerPort(const std::string &m_server_port);
private:
	std::string m_serverPort;

    ProcessStatus m_procStatus;

	std::string m_curPath;
	std::string m_file;


	CfgCtx m_vServer;
	std::string m_location;
    /* configs */
    const std::vector<CfgCtx>& m_configs;
};


} // namespace ft