//
//  BinaryReader.h
//  TerminalExtraction
//
//  Created by Mister Pattern on 11/19/12.
//
//

#ifndef __TerminalExtraction__BinaryReader__
#define __TerminalExtraction__BinaryReader__

#include <iosfwd>
#include <stdio.h>
#include <cstdint>
#include <string>
#include "BinaryInterfaces.h"



class BinaryReader : public IBinaryReader {
    
    FILE            *_file;

    const char *_currentBuffer;
    const char *_internalBuffer;
    
    long            _filesize;
    long            _position;
    
    bool            _shouldDeallocBuffer;
    
public:
    
    std::string _name;
    
    const char* GetInternalBuffer() const {return _internalBuffer;}
    
    BinaryReader(std::string filename);
    BinaryReader(const char *buffer, long length);
    BinaryReader(char *buffer, long length, bool shouldDeallocBuffer);
    ~ BinaryReader();
	bool isValid() const { return !(!_file); };
    long GetPosition();
    int ReadInt();
    unsigned int ReadUInt();
    short ReadInt16();
    unsigned short ReadUInt16();
    float ReadSingle();
    char ReadChar();
    unsigned char ReadUChar();
    std::string ReadBadString();
    std::string ReadString();
	void ReadBuffer(long length, char *buf);
    void SetPosition(long position);
    
	long GetLength() { return _filesize; };
    
    std::string ReadFullAsString();
    
#pragma mark - IBinaryReader
    
public:
    
    void ReaderSetPosition(long position) override;
    long ReaderGetPosition() override;
    long ReaderGetSize() override;
    
    int ReaderReadInt() override;
    unsigned int ReaderReadUInt() override;
    void ReaderReadBuffer(long length, char *buf) override;
    float ReaderReadFloat() override;
    
};

#endif /* defined(__TerminalExtraction__BinaryReader__) */
