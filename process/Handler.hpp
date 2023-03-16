#pragma once

#include <string>
#include <fstream>
#include <cstring>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 55

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
	private:
		size_t	m_fd;
		size_t	m_length;
		bool	m_isDone;
		size_t	m_counter;
		std::stringstream m_body;

		InputLengthHandler(){};

	public:
		InputLengthHandler(int fd, size_t length);

		virtual bool IsDone() const;

		virtual ~InputLengthHandler() {};

		virtual void ProcessInput();

        std::string GetRes() {return m_body.str();}
	};

	class InputChunkedHandler: public IInputHandler
	{
	private:
		size_t	m_fd;
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
}   //namespace ft