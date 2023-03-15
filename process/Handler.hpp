#pragma once

#include <string>
#include <fstream>
#include <cstring>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

namespace ft
{
    class IDone
    {
    public:
        virtual bool IsDone() const = 0;
        virtual ~IDone(){};
    };

    class IInputHandler: public IDone
    {
    public:
        /* save */
        virtual void ProcessInput(const std::string& chunk) = 0;
        virtual ~IInputHandler(){}
    };

    class IOutputHandler: public IDone
    {
    public:
        /* send some data from file or cgi */
        virtual void ProcessOutput() = 0;
        virtual ~IOutputHandler(){};
    };

    class OutputChunkedHandler: public IOutputHandler
    {
    private:
        int m_fd;
        std::string m_filename;
        char m_buf[BUFF_SIZE];
        bool m_isDone;
        std::ifstream   m_file;
        std::stringstream m_ss;
        OutputChunkedHandler();
        
    public:
        OutputChunkedHandler(int fd, const std::string& filename, const std::string& header);

        virtual bool IsDone() const;

        ~OutputChunkedHandler();

        void ProcessOutput();
    };
}   //namespace ft