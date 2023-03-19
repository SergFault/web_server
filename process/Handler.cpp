#include "Handler.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <exception>

namespace ft
{
    OutputChunkedHandler::OutputChunkedHandler(int fd, const std::string& filename, const std::string& header) :
            m_fd(fd),
            m_filename(filename),
            m_isDone(false)
    {
        std::fill_n(m_buf, BUFF_SIZE, '\0');

        m_ss << header;

        m_file.open(m_filename.c_str(), std::ios::in);

        if (!m_file.is_open())
        {
            throw std::runtime_error("FILE NOT OPENED");
        }

    }

    bool OutputChunkedHandler::IsDone() const
    {
        return m_isDone;
    }

    OutputChunkedHandler::~OutputChunkedHandler()
    {
        m_file.close();
        // std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<CLOSED" << std::endl;
    }

    void OutputChunkedHandler::ProcessOutput()
    {
        if (!m_file.eof() || !m_isDone)
        {
            if (m_ss.str().size() == 0)
            {
                // std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<WRITE" << std::endl;

                std::fill_n(m_buf, BUFF_SIZE, '\0');
                m_file.read(m_buf, BUFF_SIZE - 1);
                m_ss << std::hex << strlen(m_buf);
                m_ss << "\r\n" << m_buf << "\r\n";
                if (m_file.eof())
                {
                    m_ss << "0\r\n\r\n";
                }
            }
//            size_t cnt = send(m_fd, m_ss.str().c_str(), m_ss.str().size(), 0);
            size_t cnt = write(m_fd, m_ss.str().c_str(), m_ss.str().size());
            if (cnt < m_ss.str().size())
                m_ss.str(m_ss.str().substr(cnt, m_ss.str().size() - cnt));
            else
                m_ss.str("");
            if (m_file.eof() && m_ss.str().size() == 0)
                m_isDone = true;
        }
    }

    //-----------------------------------------------------------------------------------

    InputLengthHandler::InputLengthHandler(int fd, size_t length, const std::string& remain) :
                                                                m_fd(fd),
                                                                m_lengthLeft(length - remain.size()),
                                                                m_body(remain),
                                                                m_isDone(false)
                                                                { 
                                                                    std::cout << "<<<<remain" << remain.size() << std::endl;
                                                                    std::cout << "<<<<init" << length - remain.size() << std::endl;
                                                                    m_body << remain;
                                                                    m_counter = 0;
                                                                }
    
    bool InputLengthHandler::IsDone() const
    {
        return m_isDone;
    }

    void InputLengthHandler::ProcessInput()
    {
            int readRes;
            size_t bufferSize = 1024;
            char buffer[bufferSize];

            int readSize = bufferSize > m_lengthLeft ? m_lengthLeft : bufferSize ;
            readRes = recv(m_fd, buffer, readSize, 0);
            if (readRes < 0)
            {
                // return ;
                std::stringstream ss;
                ss << "error socket#" << m_fd;
                perror(ss.str().c_str());
                throw std::runtime_error("InputLengthHandler receive error");
            }
			else if (readRes == 0)
				m_isDone = true;

            m_counter += readRes;

            // std::cout << "read: " << std::endl;
            // std::cout.write(buffer, bufferSize);
            std::cout << "read all: " << m_counter << std::endl;
            std::cout << "read: " << readRes << std::endl;
            std::cout << "m_lengthLeft: " << m_lengthLeft << std::endl;

            m_lengthLeft -= readRes;

            m_body.write(buffer, readRes);

			std::cout << "[[[" << m_body.str() << "]]]" << std::endl;

            if (m_lengthLeft == 0)
            {
                m_isDone = true;
            }
    }

    std::string InputLengthHandler::GetRes()
    {
        return m_body.str();
    }



    // ----------------------------------------------------------------

    InputChunkedHandler::InputChunkedHandler(int fd, size_t max_length) :
            m_fd(fd),
            m_max_length(max_length),
            m_isDone(false),
            m_counter(0),
            m_num(0),

            finish(false)
    {

    }

    void InputChunkedHandler::ProcessInput()
    {
        char buf[BUFF_SIZE];
        std::string	body;
        std::string tmp;
        std::stringstream ss;
        size_t cnt;
        size_t pos;

        if (m_num != 0 )
        {
//			cnt = recv(m_fd, &body, m_num, 0);
            std::fill_n(buf, BUFF_SIZE, '\0');
            cnt = read(m_fd, buf, m_num < (BUFF_SIZE - 1) ? m_num : (BUFF_SIZE - 1));

            if (m_num - cnt >= 2)
                m_body << buf;
            else if (m_num > 2)
            {
                body = buf;
                m_body << body.substr(0, body.size() - (2 - (m_num - cnt)));
            }
            m_num -= cnt;
            if (m_num == 0 && finish)
                m_isDone = true;
        }
        else
        {
//			cnt = recv(m_fd, &buf, 20, 0);


            if ((pos = search_chunk.find("\r\n")) != std::string::npos)
            {
                ss << search_chunk.substr(0, pos);
                ss >> std::hex >> m_num;
                if (m_num == 0)
                    finish = true;
                m_num += 2;
                tmp = search_chunk.substr(pos + 2, search_chunk.size() - (pos - 2));
                if (tmp.size() >= m_num)
                {
                    m_body << tmp.substr(0, m_num - 2);
                    search_chunk = tmp.substr(m_num, tmp.size() - m_num);
                    m_num = 0;
                }
                else
                {
                    m_num -= tmp.size();
                    if (m_num >= 2)
                        m_body << tmp;
                    else
                        m_body << tmp.substr(0, tmp.size() - (2 - m_num));
                    search_chunk = "";
                }
            }
            else
            {
                std::fill_n(buf, BUFF_SIZE, '\0');
                cnt = recv(m_fd, buf, 20, 0);
                std::cout << "another recv" << std::endl;

                search_chunk.append(buf);
            }
        }
        if (m_num == 0 && finish)
            m_isDone = true;
    }

    bool InputChunkedHandler::IsDone() const
    {
        return m_isDone;
    }
}   //namespace ft