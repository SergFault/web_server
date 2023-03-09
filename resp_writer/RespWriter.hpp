#pragma once

#include "../resp_writer/RespUtil.hpp"

#include <sys/types.h>
#include "RespUtil.hpp"

namespace ft{

enum RespStatus
{
    Initialized,
    Processing,
    Done
};

class ResponseWriter
{
public:
    ResponseWriter();
    void readData(BufferPair& buffer);
    RespStatus getStatus() const;
private:
    RespStatus m_status;
};


} //namespace ft