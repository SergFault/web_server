#include "RespWriter.hpp"
#include <sstream>
#include <cstring>
#include <string>

namespace ft{

ResponseWriter::ResponseWriter(): m_status(Initialized){}

void ResponseWriter::readData(BufferPair& bufferPair)
{
    /* todo it`s just mocked data */
    static int counter;
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\r\n\r\n<html>\n\r<body>\n\r\rhello " << counter++ << " times\n\r</body>\n</html>";
    std::string str_data = ss.str();

    /* identifacate process begin.
    is there is a big amount of data or file - it can not be read out and send in
    one function call */
    m_status = Processing;

    std::memmove(bufferPair.data, str_data.c_str(), str_data.size());
    bufferPair.written = str_data.size();

    /* identifacate process as done */
    m_status = Done;
}

RespStatus ResponseWriter::getStatus() const
{
    return m_status;
}

} // namespace ft
