#include "Handler.hpp"

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
    }

    bool OutputChunkedHandler::IsDone() const
    {
        return m_isDone;
    }

    OutputChunkedHandler::~OutputChunkedHandler()
    {
        m_file.close();
    }

    void OutputChunkedHandler::ProcessOutput()
    {
        if (!m_file.eof() || !m_isDone)
        {
            if (m_ss.str().size() == 0)
            {
                std::fill_n(m_buf, BUFF_SIZE, '\0');
                m_file.read(m_buf, BUFF_SIZE - 1);
                m_ss << std::hex << strlen(m_buf);
                m_ss << "\r\n" << m_buf << "\r\n";
                if (m_file.eof())
                {
                    m_ss << "0\r\n\r\n";
                }
            }
            size_t cnt = send(m_fd, m_ss.str().c_str(), m_ss.str().size(), 0);
            if (cnt < m_ss.str().size())
                m_ss.str(m_ss.str().substr(cnt, m_ss.str().size() - cnt));
            else
                m_ss.str("");
            if (m_file.eof() && m_ss.str().size() == 0)
                m_isDone = true;
        }
    }
}   //namespace ft

// int main()
// {
//     int fd = open("outfile.txt", O_CREAT | O_WRONLY);

//     OutputChunkedHandler hndlr(fd, "../www/default/index.html");

//     while (!hndlr.IsDone())
//     {
//         hndlr.ProcessOutput();
//         // sleep(5);
//     }

//     close(fd);

//     return 0;
// }