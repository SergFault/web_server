#include "Handler.hpp"

namespace ft
{
    OutputChunkedHandler::OutputChunkedHandler(int fd, const std::string& filename, const std::string& header) :
            m_fd(fd),
            m_filename(filename),
            m_isDone(false)
    {
        m_ss << header;

        m_file.open(m_filename.c_str(), std::ios::in);

        if (!m_file.is_open())
        {
            if (errno == ENOENT) {
                m_file.open("../www/default/error_pages/404.html");
                m_ss.str("");
                m_ss << "HTTP/1.1 404 Not found\r\nTransfer-Encoding: chunked\r\nConnection: closed\r\n\r\n";
            }
            else
            {
                m_file.open("../www/default/error_pages/500.html");
                m_ss.str("");
                m_ss << "HTTP/1.1 500 Internal server error\r\nTransfer-Encoding: chunked\r\nConnection: closed\r\n\r\n";
            }
            //throw std::runtime_error("FILE NOT OPENED");
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
            if (m_ss.str().empty())
            {
                // std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<WRITE" << std::endl;
                m_file.read(m_buf, BUFF_SIZE);
                std::streamsize size = m_file.gcount();
                m_ss << std::hex << size;
                m_ss << "\r\n";
                m_ss.write(m_buf, size);
                m_ss << "\r\n";
                if (m_file.eof())
                {
                    m_ss << "0\r\n\r\n";
                }
            }
            size_t cnt = send(m_fd, m_ss.str().c_str(), m_ss.str().size(), 0);
//            size_t cnt = write(m_fd, m_ss.str().c_str(), m_ss.str().size());
            if (cnt < m_ss.str().size())
                m_ss.str(m_ss.str().substr(cnt, m_ss.str().size() - cnt));
            else
                m_ss.str("");
            if (m_file.eof() && m_ss.str().empty())
                m_isDone = true;
        }
    }

	OutputRawHandler::OutputRawHandler(int fd, const std::string& text) :
		m_fd(fd),
		m_isDone(false),
		m_text(text)
	{
		m_ss << text;
	}

	bool OutputRawHandler::IsDone() const
	{
		return m_isDone;
	}

	OutputRawHandler::~OutputRawHandler()
	{
	}

	void OutputRawHandler::ProcessOutput()
	{
		if (!m_isDone)
		{
			size_t cnt = send(m_fd, m_text.c_str(),
							  m_text.size() > BUFF_SIZE ? BUFF_SIZE : m_text.size(), 0);
			if (cnt < m_text.size())
				m_text = m_text.substr(cnt, m_text.size() - cnt);
			else
				m_text.clear();
			if (m_ss.str().empty())
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
//            std::cout << "read all: " << m_counter << std::endl;
//            std::cout << "read: " << readRes << std::endl;
//            std::cout << "m_lengthLeft: " << m_lengthLeft << std::endl;

            m_lengthLeft -= readRes;

            m_body.write(buffer, readRes);

//			std::cout << "[[[" << m_body.str() << "]]]" << std::endl;

            if (m_lengthLeft == 0)
            {
                std::cout << m_body.str() << std::endl;//debug
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

    InputCgiPostHandler::InputCgiPostHandler(char** envp[13], char** argv[2],
                                             const std::string &query)
                                             :  m_isDone(false)
    {
        m_ss << query;
        pipe(m_pipe_to_cgi);
        pipe(m_pipe_from_cgi);

        m_pid = fork();
        if (m_pid == -1)
        {
            //throw error 500
        }
        else if (m_pid == 0)
        {
            //fork
            dup2(m_pipe_to_cgi[0], 0);
            close(m_pipe_to_cgi[0]);
            close(m_pipe_to_cgi[1]);
            dup2(m_pipe_from_cgi[1], 1);
            close(m_pipe_from_cgi[0]);
            close(m_pipe_from_cgi[1]);
			//execve

			if (execve((*argv)[0], *argv, *envp) == -1)
			{
				//error 500;
			}
			_exit(-1);
        }
        else if (m_pid > 0) {
            //this
			std::time(&m_timer);
            close(m_pipe_to_cgi[0]);
            close(m_pipe_from_cgi[1]);
//            while (1)
//            {
//                if (std::time(NULL) - m_timer > 15)
//                {
//                    std::cout << "time out\n";
//                    kill(m_pid, SIGKILL); // throw error 504
//                    break;
//                }
//            }
        }
    }

    void InputCgiPostHandler::ProcessInput()
    {
		int status;
        if (!m_isDone)
        {
			if (waitpid(m_pid, &status, WNOHANG) != m_pid)
			{
				if (std::time(NULL) - m_timer > 15)
				{
					std::cout << "time out\n";
					kill(m_pid, SIGKILL); // throw error 504
					m_isDone = true;
				}
				size_t len = read(m_pipe_from_cgi[1], m_buf, BUFF_SIZE);
				m_ss.write(m_buf, len);
			}
			else
			{
				size_t len = read(m_pipe_from_cgi[0], m_buf, BUFF_SIZE);
				m_ss.write(m_buf, len);
				close(m_pipe_from_cgi[0]);
				close(m_pipe_to_cgi[1]);
				m_isDone = true;
			}
        }
    }

    bool InputCgiPostHandler::IsDone() const {
        return m_isDone;
    }
}   //namespace ft