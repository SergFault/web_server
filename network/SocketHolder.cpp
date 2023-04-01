#include "SocketHolder.h"
#include <arpa/inet.h>
#include <bitset>
#include <fcntl.h>
#include "../utils/utils.hpp"//

#define CHUNKED_HEADER "HTTP/1.1 200 OK\r\n"\
"Transfer-Encoding: chunked\r\n"\
"Connection: keep-alive\r\n"\
"\r\n"                                      \

#define ERROR_404_HEADER "HTTP/1.1 404 Not found\r\n"\
"Transfer-Encoding: chunked\r\n"\
"Connection: close\r\n"\
"\r\n"

namespace ft
{

SocketHolder::SocketHolder(int desc, const std::vector<CfgCtx>& ctxs) :
                                        m_file_descriptor(desc),
                                        m_configs(ctxs),
                                        m_hostSockAddrLen(0)
{
    m_procStatus = ReadRequest;
    memset(&m_hostSockAdd, 0, sizeof(m_hostSockAdd));
}

int SocketHolder::getFd()
{
    return m_file_descriptor;
}

const std::string& SocketHolder::getServerIp() const
{
    return m_serverIp;
}

const std::string& SocketHolder::getServerPort() const
{
    return m_serverPort;
}

ProcessStatus SocketHolder::getStatus() const
{
    return m_procStatus;
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

    flags |= O_NONBLOCK;
    if (fcntl(m_file_descriptor, F_SETFL, O_NONBLOCK) <  0)
    {
        throw std::runtime_error("Can`t set nonblocking option \n");
    }
}

SocketHolder::SocketHolder(int domain, int type, int protocol, const std::vector<CfgCtx>& ctxs) :
                                                                        m_procStatus(ReadRequest),
                                                                        m_configs(ctxs)
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
}

void SocketHolder::bind(struct sockaddr_in *addr)
{
     if (::bind(m_file_descriptor, reinterpret_cast<const struct sockaddr *>(addr), sizeof(*addr)) == -1)
     {
        std::cout << m_file_descriptor << std::endl;
        perror("error:\n");
        throw std::runtime_error("Error while binding\n");
     }

    std::stringstream ss;
    ss << ntohs(addr->sin_port);

    m_serverPort = ss.str();
    m_serverIp = std::string(inet_ntoa((addr)->sin_addr));

    // std::cout << "binding" << m_serverIp << " " << m_serverPort << std::endl;
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



// void SocketHolder::SetNextState()
// {
//     switch (m_procStatus)
//     {
//         case (ReadRequest):
//             std::cout << "now STEP: ReadBody" << std::endl;
//             m_procStatus = ReadBody;
//             break;
//         case (ReadBody):
//             std::cout << "now STEP: ReadDone" << std::endl;
//             m_procStatus = ReadDone;
//             break;
//         case (ReadDone):
//             std::cout << "now STEP: WriteRequest" << std::endl;
//             m_procStatus = WriteRequest;
//             break;
//         case (WriteRequest):
//             std::cout << "now STEP: WriteBody" << std::endl;
//             m_procStatus = WriteBody;
//             break;
//         case (WriteBody):
//             std::cout << "now STEP: WriteRequest" << std::endl;
//             m_procStatus = WriteRequest;
//             break;
//         case (Done):
//             std::cout << "now STEP: Done" << std::endl;
//             break;
//     }

// }

void SocketHolder::ProcessRead()
{
	if (m_procStatus == ReadRequest)
	{
		std::cout << "  socket #" << m_file_descriptor << " AccumulateRequest" << std::endl;
		AccumulateRequest();
	}
	else if (m_procStatus == ReadBody)
	{
		std::cout << "  socket #" << m_file_descriptor << " HandleBody" << std::endl;
		HandleBody();
	}
//	else if (m_procStatus == PrepareCgi)
//	{
//		SetCgi();
//	}
//	else if (m_procStatus == ProcessCgi)
//	{
//
//	}

}

void SocketHolder::InitWriteHandler()
{
	if (m_reqHeader->get_req_headers().is_cgi)
	{
		m_writeHandler = Shared_ptr<IOutputHandler>(new OutputRawHandler(m_file_descriptor,
																		 m_cgi_raw_out));
	}
    else if (m_writeHandler.get() == NULL)
    {
        m_writeHandler = Shared_ptr<IOutputHandler>(new OutputChunkedHandler(m_file_descriptor,
                                                    m_file,
                                                    CHUNKED_HEADER));
    }
}

void SocketHolder::ProcessWrite()
{

    /* to do make desition depending on state*/
    if (m_procStatus == WriteRequest)
    {
        InitWriteHandler();
        m_writeHandler->ProcessOutput();
        if (m_writeHandler->IsDone())
        {
            std::cout << "  socket #" << m_file_descriptor << "ProcessWrite" << "status = Done" << std::endl;
            m_procStatus = Done;
        }
    }
	else if (m_procStatus == PrepareCgi)
	{
		SetCgi();
	}
	else if (m_procStatus == ProcessCgi)
	{
		HandleCgi();
	}
}

Shared_ptr<SocketHolder> SocketHolder::accept()
{
    int res = ::accept(m_file_descriptor, &m_hostSockAdd, &m_hostSockAddrLen);

    // std::cout << res << std::endl;

    // SocketHolder sock(res);
    Shared_ptr<SocketHolder> sock(new SocketHolder(res, m_configs));

    // std::cout << "Status::: " << std::endl;

    /* todo debug */
    if (sock->getFd() != -1)
    {
//		std::stringstream ss;
//		ss << ntohs(reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)->sin_port);
//		ss >> m_serverPort;
//		m_serverIp = std::string(inet_ntoa(reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)->sin_addr));
		// m_ip_port = std::string(inet_ntoa(reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)->sin_addr))
		// 			+ ":";
		// m_ip_port.append(ss.str());
        std::cout << "NEW SOCKET ACCEPTED" << std::endl;
        std::cout << "  Host Port:" << m_serverIp << " : " << m_serverPort << std::endl;
        std::cout << "  FD:" << sock->getFd() << std::endl << std::endl;
	}
    else
    {
    // std::cout << reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)-> << std::endl;
        throw std::runtime_error("Error accepting socket");
    }

    return sock;
}

SocketHolder::~SocketHolder()
{

    std::cout << std::endl << "<<<<<<<socket destroyed " << m_file_descriptor << std::endl;
    std::cout << "          m_req_string:" <<  m_req_string << std::endl;
    std::cout << "          m_remainAfterRequest:" << m_remainAfterRequest << std::endl << std::endl;

    // std::cout << "free " << m_file_descriptor << " objc " << *m_obj_counter <<  std::endl;
    if (m_file_descriptor != -1)
    {
        ::shutdown(m_file_descriptor, SHUT_RDWR);
        ::close(m_file_descriptor);
    }
}

void SocketHolder::AccumulateRequest()
{
    char buffer[BUFF_SIZE];
   
    int res = recv(m_file_descriptor, buffer, BUFFER_SIZE, 0);

    if (res < 0)
    {
        // std::cout << m_file_descriptor << std::endl;
        perror("FAILED");
        throw std::runtime_error("AccumulateRequest FAILED");
    }

	m_req_string.append(buffer, res);

    std::string::size_type found = m_req_string.find("\r\n\r\n");

    /* if req header done reading */
    if (found != std::string::npos)
    {
		m_remainAfterRequest = m_req_string.substr(found + 4, m_req_string.size() - (found + 4));

//        std::cout << "remain string:" << m_remainAfterRequest.size() <<std::endl;

		m_req_string = m_req_string.substr(0, found);
        /*todo testing only. should be READ BODY*/
        m_reqHeader = Shared_ptr<HttpReqHeader>(new HttpReqHeader(m_req_string));
//        m_procStatus = WriteBody;

		if (m_reqHeader->get_req_headers().method == "POST")
        {
			m_procStatus = ReadBody;
            InitBodyHandler();
        }
		else if (m_reqHeader->get_req_headers().method == "GET")
        {
			if (m_reqHeader->get_req_headers().is_cgi)
				m_procStatus = PrepareCgi;
			else
				m_procStatus = WriteRequest;
        }

		SetVServer();
		SetLocation();
    }
}

 void SocketHolder::InitBodyHandler()
 {
    //   std::cout << "<<<<<<<<INIT BODYHANDLER" << std::endl;
     if (m_bodyHandler.get() == NULL)
     {
        if((*m_reqHeader).get_req_headers().cont_length > 0)
        {
		 m_bodyHandler = Shared_ptr<IInputHandler>(new InputLengthHandler
			 (m_file_descriptor, m_reqHeader->get_req_headers().cont_length, m_remainAfterRequest));
         std::cout << "  socket #" << m_file_descriptor << "<<<<<<<<INIT BODYHANDLER InputLengthHandler" << std::endl;
        }
        else if((*m_reqHeader).get_req_headers().cont_length == 0)
        {
            if ((*m_reqHeader).get_req_headers().is_chunked)
            {
                std::cout << "  socket #" << m_file_descriptor << "<<<<<<<<INIT BODYHANDLER InputLengthHandler" << std::endl;
                m_bodyHandler = Shared_ptr<IInputHandler>(new InputChunkedHandler
			        (m_file_descriptor, 1000000));
            }
            else
            {
                // std::cout << "<<<<<<SET STATUS DONE" << std::endl;
                std::cout << "  socket #" << m_file_descriptor << "status = Done" << std::endl;
                m_procStatus = Done;
            }
        }
        else
        {
            std::cout << "  socket #" << m_file_descriptor << "status = Done" << std::endl;
            m_procStatus = Done;
        }
     }
 }

 void SocketHolder::HandleBody()
 {
     InitBodyHandler();

     std::cout << "HANDLE BODY" << std::endl;
     m_bodyHandler->ProcessInput();
     if (m_bodyHandler->IsDone())
     {
		 m_body = dynamic_cast<InputLengthHandler *>(m_bodyHandler.get())->GetRes();
		 if (!m_reqHeader->get_req_headers().is_cgi && !m_reqHeader->get_req_headers().boundary.empty())
		 {
			 std::string filename;

			 size_t begin, end;
			 //std::string s = dynamic_cast<InputLengthHandler *>(m_bodyHandler.get())->GetRes();

			 begin = m_body.find("; filename=\"");

			 end = m_body.find("\r\n", begin);

			 filename = m_body.substr(begin + sizeof("; filename="), end - begin - sizeof("; filename=") - 1);

			 begin = m_body.find("\r\n\r\n", end) + 4;
			 end = m_body.find("--" + m_reqHeader->get_req_headers().boundary, begin);

			 std::fstream file(filename.c_str(), std::ios::out);
			 for (size_t i = begin; i < end; ++i) {
				 file.put(m_body[i]);
			 }
			 file.close();
		 }
		 if (m_reqHeader->get_req_headers().is_cgi)
			 m_procStatus = PrepareCgi;
		 else
			 m_procStatus = WriteRequest;//
     }
 }

void SocketHolder::SetVServer()
{
	std::vector<CfgCtx>::const_iterator cit;
	std::vector<CfgCtx>::iterator it;
	std::vector<CfgCtx> match;
	std::string		host = m_reqHeader->get_req_headers().host;

	std::string	server_ip_port;
	std::string m_ip_port = m_serverIp + ":" + m_serverPort;

	for (cit = m_configs.begin(); cit != m_configs.end(); ++cit)
	{
		server_ip_port = cit->ip + ":" + cit->port;

		if ((server_ip_port == m_ip_port) || (cit->ip == "0.0.0.0" && (cit->port == m_serverPort)))
			match.push_back(*cit);
	}

	for (it = match.begin(); it != match.end(); ++it)
	{
		if (it->server_names.find(host) != it->server_names.end())
		{
			m_vServer = *it;
			return;
		}
	}

	m_vServer = *match.begin();
}

void SocketHolder::SetLocation()
{
	m_file = m_reqHeader->get_req_headers().path;
	bool isdir = m_reqHeader->get_req_headers().is_req_folder;

	std::set<std::string>::reverse_iterator it;

	for (it = m_vServer.location_paths.rbegin(); it != m_vServer.location_paths.rend(); ++it)
	{
		std::cout << "m_file " << m_file << " | location: " << *it << std::endl;
		if (m_file.find(*it) != std::string::npos)
			m_location = *it;
	}

	if (m_location.empty())
		std::cout << "Error to find location" << std::endl;
	else
		std::cout << "Found location: [" << m_location << "]" << std::endl;

	if (isdir)
	{
		m_file = m_vServer.locations.find(m_location)->second.root
			+ "/" + m_vServer.locations.find(m_location)->second.index;
	}
	else
		m_file = m_vServer.locations.find(m_location)->second.root + m_file;//.substr(1);

	std::cout << "[[" << m_file << "]]";
}

void SocketHolder::SetMServerIp(const std::string &m_server_ip)
{
	m_serverIp = m_server_ip;
}

void SocketHolder::SetMServerPort(const std::string &m_server_port)
{
	m_serverPort = m_server_port;
}
void SocketHolder::SetCgi()
{
	request_headers hdrs = m_reqHeader->get_req_headers();
	m_envp[0] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	m_envp[1] = strdup(("PATH_INFO=" + hdrs.path_info).c_str());
	m_envp[2] = strdup(("QUERY_STRING=" + hdrs.query).c_str());
	m_envp[3] = strdup(("REQUEST_METHOD=" + hdrs.method).c_str());
	m_envp[4] = strdup(("SCRIPT_NAME=" + hdrs.path).c_str());
	m_envp[5] = strdup(("SERVER_NAME=" + hdrs.host).c_str());
	m_envp[6] = strdup(("SERVER_PORT=" + hdrs.port_str).c_str());
	m_envp[7] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	char buf[1024];
	getcwd(buf, 1024);
	std::string wd = buf;
	m_envp[8] = strdup(("PATH_TRANSLATED="
						+ wd + "/" + m_vServer.locations.find(m_location)->second.root
						+ hdrs.path_info).c_str());
	m_envp[9] = strdup(("SCRIPT_FILENAME="
							+ m_vServer.locations.find(m_location)->second.root
							+ hdrs.path).c_str());
	m_envp[10] = strdup(("CONTENT_TYPE=" + hdrs.content_type).c_str());
	if (hdrs.cont_length != 0)
		m_envp[11] = strdup(("CONTENT_LENGTH=" + to_string(hdrs.cont_length)).c_str());
	else if (hdrs.method == "POST")
	{
		size_t cont_length = m_body.size();
		if (cont_length > 0)
			m_envp[11] = strdup(("CONTENT_LENGTH=" + to_string(cont_length)).c_str());
	}
	else
		m_envp[11] = strdup("CONTENT_LENGTH=");
	m_envp[12] = NULL;

	m_argv[0] = strdup((m_vServer.locations.find(m_location)->second.root + hdrs.path).c_str());
	m_argv[1] = NULL;

	m_cgiHandler = Shared_ptr<IInputHandler>(new InputCgiPostHandler(reinterpret_cast<char ***>(&m_envp), reinterpret_cast<char ***>(&m_argv), hdrs.query));
	m_procStatus = ProcessCgi;
}
void SocketHolder::HandleCgi()
{
	m_cgiHandler->ProcessInput();
	if (m_cgiHandler->IsDone())
	{
		for (size_t i = 0; m_envp[i] != NULL; ++i)
		{
			delete (m_envp[i]);
		}
		delete m_argv[0];
		m_cgi_raw_out = dynamic_cast<InputCgiPostHandler *>(m_cgiHandler.get())->GetRes();
		m_procStatus = WriteRequest;
	}
}

} //namespace ft