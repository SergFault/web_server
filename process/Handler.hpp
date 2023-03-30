#pragma once

#include <string>
#include <fstream>
#include <cstring>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <ctime>

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <exception>
#include <sys/wait.h>

#include <cstdlib>

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
        virtual void ProcessInput() = 0;
		virtual ~IInputHandler(){};
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
        OutputChunkedHandler(){};
        
    public:
        OutputChunkedHandler(int fd, const std::string& filename, const std::string& header);

        virtual bool IsDone() const;

        virtual ~OutputChunkedHandler();

        virtual void ProcessOutput();
    };

	class InputLengthHandler: public IInputHandler
	{
	public:
		InputLengthHandler(int fd, size_t length, const std::string& remain);

		virtual bool IsDone() const;

		virtual ~InputLengthHandler() {};

		virtual void ProcessInput();

        std::string GetRes();

	private:
		size_t	m_fd;
		size_t	m_lengthLeft;
		bool	m_isDone;
		size_t	m_counter;
		std::stringstream m_body;

		std::string m_str; //debug

		InputLengthHandler(){};
	};

	class InputChunkedHandler: public IInputHandler
	{
	private:
		int	    m_fd;
		size_t	m_max_length;
		bool	m_isDone;
		size_t	m_counter;
		size_t 	m_num;
		std::stringstream m_body;

		std::string search_chunk;
		bool finish;

		InputChunkedHandler(){};

	public:
		InputChunkedHandler(int fd, size_t max_length);

		virtual bool IsDone() const;
		virtual ~InputChunkedHandler() {};
		virtual void ProcessInput();

		std::string GetRes() {return m_body.str();}
	};

    class InputCgiPostHandler: public IInputHandler
    {
    private:
        pid_t   m_pid;
        time_t  m_timer;
        int     m_pipe_to_cgi[2];
        int     m_pipe_from_cgi[2];
        std::string m_cgi_response;
        bool    m_isDone;
        size_t  m_content_length;
        std::stringstream m_ss;

    public:
        InputCgiPostHandler(int fd, size_t length, const std::string& query, const std::string& path);

        virtual bool IsDone() const;

        virtual void ProcessInput();

    };
}   //namespace ft