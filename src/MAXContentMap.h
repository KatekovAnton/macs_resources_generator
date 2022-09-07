//
//  MAXContentMap.h
//  MAX
//
//  Created by Anton Katekov on 26.12.12.
//  Copyright (c) 2012 AntonKatekov. All rights reserved.
//

#ifndef __MAX__MAXContentMap__
#define __MAX__MAXContentMap__

#include "Geometry.h"
#include <string>



using namespace std;

typedef enum
{
    ConvertPaletteTypeNone,
    ConvertPaletteTypeSepia,
    ConvertPaletteTypeBlackAndWhite,
    ConvertPaletteTypeGrayscaled,
    ConvertPaletteTypeProjection
} ConvertPaletteType;


void ConvertPalette(unsigned char &r, unsigned char &g, unsigned char &b);
void ConvertPalette(unsigned char &r, unsigned char &g, unsigned char &b, ConvertPaletteType type);
void ConvertPalette(Color &color);

class BinaryReader;
class ByteBuffer;

class MAXContentMap {
    
public:
    
    std::string filename;
    std::string name;
    
    int w, h;
    short elementCount;
    
    short* map;
    char* groundType;//0-ground 1-water 2-coast 3-unpassable
    
    //пиксели тайлов
    char* mapElements;
    char* minimap;
    Color* palette;
    ConvertPaletteType convertPaletteType;
    
    MAXContentMap();
    ~MAXContentMap();
    
    void LoadShortFromBuffer(ByteBuffer &buffer);
    void LoadFromBuffer(ByteBuffer &buffer);
    
    std::string GetMapId()
    {
        return filename.substr(0, filename.length() - 4);
    }
};

#endif /* defined(__MAX__MAXContentMap__) */
