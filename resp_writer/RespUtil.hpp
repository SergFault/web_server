#pragma once

#include <sys/types.h>

#define BUFFER_SIZE 1024

struct BufferPair
{
    char data[BUFFER_SIZE];
    size_t written;
};