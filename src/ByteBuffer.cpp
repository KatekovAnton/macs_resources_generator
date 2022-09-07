//
//  ByteBuffer.cpp
//  MAX
//
//  Created by Anton Katekov on 25.04.14.
//  Copyright (c) 2014 AntonKatekov. All rights reserved.
//

#include "ByteBuffer.h"
#include <cstdlib>
#include <cstring>

ByteBuffer::ByteBuffer()
	:_step(1024)
{
    _size = 0;
    _dataSize = 1024;
	_data = (unsigned char*)malloc(_step);
    memset(_data, 0, _step);
}

ByteBuffer::ByteBuffer(size_t step)
:_step(step)
{
    _size = 0;
    _dataSize = _step;
    _data = (unsigned char*)malloc(_step);
    memset(_data, 0, _step);
}

ByteBuffer::~ByteBuffer()
{
    free(_data);
}

unsigned char* ByteBuffer::getPointer()
{
    return _data;
}

unsigned char* ByteBuffer::getPointer() const
{
    return _data;
}

size_t ByteBuffer::getDataSize() const
{
    return _size;
}

size_t ByteBuffer::getFullSize() const
{
    return _dataSize;
}

void ByteBuffer::dataAppended(size_t size)
{
    _size += size;
}

size_t ByteBuffer::getAvailableSize() const
{
    return _dataSize - _size;
}

void ByteBuffer::appendData(const unsigned char* buffer, size_t size, size_t nitems)
{
    size_t rembuff = 0;
    size *= nitems;
    rembuff= _dataSize - _size; /* remaining space in buffer */
    
    if(size > rembuff)
    {
        increaseBufferBy(size - rembuff);
    }
    memcpy(_data + _size, buffer, size);
    _size = size + _size;
}

void ByteBuffer::increaseBuffer()
{
    increaseBufferBy(_step);
}

void ByteBuffer::increaseBufferBy(size_t additionalPlace)
{
    unsigned int times = additionalPlace / _step + 1;
    additionalPlace = times * _step;
    unsigned char *newbuff = (unsigned char*)realloc((void*)_data, _dataSize + additionalPlace);
    
    _dataSize = _dataSize + additionalPlace;
    _data = newbuff;
}
