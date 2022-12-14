//
//  BinaryReader.cpp
//  TerminalExtraction
//
//  Created by Mister Pattern on 11/19/12.
//
//
#include <memory>
#include <string>
#include <string.h>

#include "BinaryReader.h"



using namespace std;

BinaryReader::BinaryReader(std::string filename):
_position(0),
_filesize(0),
_name(filename),
_shouldDeallocBuffer(false)
{

	std::string path = filename;
    _file = fopen(path.c_str(), "rb");
    if (_file)
    {
        fseek(_file, 0, SEEK_END);
        _filesize = ftell(_file);
        fseek(_file, 0, SEEK_SET);
    }
    else
    {
        _filesize = 0;
    }

//    _currentBuffer = static_cast<char*>(malloc(_filesize));
}

BinaryReader::BinaryReader(const char *buffer, long length):
_position(0),
_filesize(length),
_file(NULL),
_shouldDeallocBuffer(false)
{
    _internalBuffer = buffer;
    SetPosition(0);
}


BinaryReader::BinaryReader(char *buffer, long length, bool shouldDeallocBuffer)
:_position(0),
_filesize(length),
_file(NULL),
_shouldDeallocBuffer(shouldDeallocBuffer)
{
    _internalBuffer = buffer;
    SetPosition(0);
}

long BinaryReader::GetPosition()
{
    if (_file)
    {
#ifdef ANDROID
    	off_t asset_l = AAsset_getLength(_file);
		off_t asset_r = AAsset_getRemainingLength(_file);
	    _position = asset_l - asset_r;
#else
        _position = ftell(_file);
#endif
    }
    return _position;
}

void BinaryReader::SetPosition(long position)
{
    if (position <= _filesize)
    {
        _position = position;
    }
    if (_file)
    {
#ifdef ANDROID
    	AAsset_seek(_file, _position, SEEK_SET);
#else
        fseek(_file, _position, SEEK_SET);
#endif
    }
    else
    {
        _currentBuffer = _internalBuffer;
        _currentBuffer += _position;
     //   _currentBuffer = *&_internalBuffer + _position; //reinterpret_cast<char *>(_internalBuffer)> + _position;
    }
}


int BinaryReader::ReadInt()
{
    if (_file)
    {
        int retVal;
        fread(&retVal, sizeof(int), 1, _file);
        return retVal;
    }
    int retVal = 0;
    memcpy(&retVal, _currentBuffer, sizeof(retVal));
    _currentBuffer += sizeof(int);
    _position += sizeof(int);
    return retVal;
}

unsigned int BinaryReader::ReadUInt()
{
    if (_file)
    {
        unsigned int retVal;
        fread(&retVal, sizeof(unsigned int), 1, _file);
        return retVal;
    }
    unsigned int retVal = 0;
    memcpy(&retVal, _currentBuffer, sizeof(retVal));
    _currentBuffer += sizeof(unsigned int);
    _position += sizeof(unsigned int);
    return retVal;
}

unsigned short BinaryReader::ReadUInt16()
{
    if (_file)
    {
        unsigned short retVal;
        fread(&retVal, sizeof(unsigned short), 1, _file);
        return retVal;
    }
    unsigned short retVal = 0;
    memcpy(&retVal, _currentBuffer, sizeof(retVal));
    _currentBuffer += sizeof(unsigned short);
    _position += sizeof(unsigned short);
    return retVal;
}

short BinaryReader::ReadInt16()
{
    if (_file)
    {
        short retVal;
        fread(&retVal, sizeof(short), 1, _file);
        return retVal;
    }
    short retVal = 0;
    memcpy(&retVal, _currentBuffer, sizeof(retVal));
    _currentBuffer += sizeof(short);
    _position += sizeof(short);
    return retVal;
}

float BinaryReader::ReadSingle()
{
    if (_file)
    {
        float retVal;
        fread(&retVal, sizeof(float), 1, _file);
        return retVal;
    }
    float retVal;// = *reinterpret_cast<float*>(_currentBuffer);
    memcpy(&retVal, _currentBuffer, 4);
    _currentBuffer += 4;
    _position += 4;
    return retVal;
}

unsigned char BinaryReader::ReadUChar()
{
    if (_file)
    {
        unsigned char retVal;
        fread(&retVal, sizeof(unsigned char), 1, _file);
        return retVal;
    }
    unsigned char retVal = *_currentBuffer;
    _currentBuffer += 1;
    _position += 1;
    return retVal;
}

char BinaryReader::ReadChar()
{
    if (_file)
    {
        char retVal;
        fread(&retVal, sizeof(char), 1, _file);
        return retVal;
    }
    char retVal = *_currentBuffer;
    _currentBuffer += 1;
    _position += 1;
    return retVal;
}

void BinaryReader::ReadBuffer(long length, char *buf)
{
    if (_file)
    {
        fread(buf, sizeof(char), length, _file);
        return;
    }
    memcpy(buf, _currentBuffer, length);
    _currentBuffer += length;
    _position += length;
    return;
}

string BinaryReader::ReadBadString()
{
    int length = ReadInt();
    char *data = (char*) malloc (sizeof(char)*length+1);
    ReadBuffer(length+1, data);
    string str = string(data);
    free(data);
    return str;
}

string BinaryReader::ReadString()
{
    int length = ReadInt();
    char *data = (char*) malloc (sizeof(char)*length+1);
    ReadBuffer(length+1, data);
    if (data[length] != '\0')
    {
        ReadChar();
        char *dataold = data;
        data = (char*) malloc (sizeof(char)*length+2);
        memcpy(data, dataold, length+1);
        data[length+1]='\0';
        free(dataold);
    }
    string str = string(data);
    free(data);
    return str;
}

BinaryReader::~BinaryReader()
{
    if (_file)
    {
#ifdef ANDROID
    	AAsset_close(_file);
#else
        fclose(_file);
#endif
        return;
    }
    if (_currentBuffer && _shouldDeallocBuffer)
        free(const_cast<char *>(_currentBuffer));
}

std::string BinaryReader::ReadFullAsString()
{
    SetPosition(0);
    long length = GetLength();
    char* data = (char*)malloc(length+2);
    memset(data, 0, length+2);
    ReadBuffer(length, data);
    data[length] = '\0';
    data[length+1] = '\0';
    std::string result = string(data);
    free(data);
	//removeBadCharacters(result);
    return result;
}

#pragma mark - IBinaryReader

void BinaryReader::ReaderSetPosition(long position)
{
    SetPosition(position);
}

long BinaryReader::ReaderGetPosition()
{
    return GetPosition();
}

long BinaryReader::ReaderGetSize()
{
    return GetLength();
}

int BinaryReader::ReaderReadInt()
{
    return ReadInt();
}

unsigned int BinaryReader::ReaderReadUInt()
{
    return ReadUInt();
}

void BinaryReader::ReaderReadBuffer(long length, char *buf)
{
    ReadBuffer(length, buf);
}

float BinaryReader::ReaderReadFloat()
{
    return ReadSingle();
}
