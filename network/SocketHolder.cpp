#include "SocketHolder.h"
#include <arpa/inet.h>
#include <bitset>
#include <fcntl.h>

#define CHUNKED_HEADER "HTTP/1.1 200 OK\r\n"\
"Content-Type: text/html\r\n"\
"Transfer-Encoding: chunked\r\n"\
"Connection: keep-alive\r\n"\
"\r\n"\

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
    if (fcntl(m_file_descriptor, F_SETFL, flags) < 0)
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
	std::string resChunk;

    int res = recv(m_file_descriptor, m_buffer, BUFFER_SIZE - 1, 0);
    m_buffer[BUFFER_SIZE - 1] = '\0';

    if (res < 0)
    {
        // std::cout << m_file_descriptor << std::endl;
        perror("FAILED");
        throw std::runtime_error("ProcessRead FAILED");
    }

    m_buffer[res] = '\0';
    /*std::string*/ resChunk = m_buffer;//(m_buffer);
	
    if (m_procStatus == ReadRequest)
    {
        std::cout << "  socket #" << m_file_descriptor << " AccumulateRequest" << std::endl;
        AccumulateRequest(resChunk);
    }
    if (m_procStatus == ReadBody)
    {
        std::cout << "  socket #" << m_file_descriptor << " HandleBody" << std::endl;
		HandleBody();
    }

}

void SocketHolder::InitWriteHandler()
{
    if (m_writeHandler.get() == NULL)
    {
        // std::cout << "InitWriteHandler" << std::endl;
        m_writeHandler = Shared_ptr<IOutputHandler>(new OutputChunkedHandler(m_file_descriptor,
                                                    "../www/default/index.html",
                                                    CHUNKED_HEADER));
        // std::cout << "InitWriteHandler done" << std::endl;
    }
}

void SocketHolder::ProcessWrite()
{

    // std::cout << "ProcessWrite" << m_file_descriptor << std::endl;
    /* to do make desition depending on state*/
    if (true)
    {
        InitWriteHandler();
        m_writeHandler->ProcessOutput();
        if (m_writeHandler->IsDone())
        {
            std::cout << "  socket #" << m_file_descriptor << "ProcessWrite" << "status = Done" << std::endl;
            m_procStatus = Done;
        }
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
        std::cout << "NEW SOCKET ACCEPTED" << std::endl;
        std::cout << "  Host Address:" << inet_ntoa(reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)->sin_addr) << std::endl;
        std::cout << "  FD:" << sock->getFd() << std::endl;
        std::cout << "  Host Port:" << reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)->sin_port << std::endl << std::endl;
    }
    // std::cout << reinterpret_cast<sockaddr_in*>(&m_hostSockAdd)-> << std::endl;


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

void SocketHolder::AccumulateRequest(const std::string& str)
{
    std::string::size_type found = str.find("\r\n\r\n");
    m_req_string.append(str);

    /* if req header done reading */
    if (found != std::string::npos)
    {
		m_remainAfterRequest = m_req_string.substr(found + 4, m_req_string.size() - (found + 4));
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
			m_procStatus = WriteRequest;
        }
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
         m_procStatus = Done;
		 std::string s = dynamic_cast<InputLengthHandler*>(m_bodyHandler.get())->GetRes();
		 std::fstream file("body.txt", std::ios::out);
		 for (size_t i = 0; i < s.size(); ++i)
		 {
			 file.put(s[i]);
		 }
		 file.close();
		 m_procStatus = WriteRequest;//
     }
 }


} //namespace ft