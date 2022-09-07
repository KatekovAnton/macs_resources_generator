//
//  MAXContentMap.cpp
//  MAX
//
//  Created by Anton Katekov on 26.12.12.
//  Copyright (c) 2012 AntonKatekov. All rights reserved.
//

#include "MAXContentMap.h"
#include "BinaryReader.h"
#include "ByteBuffer.h"

const int pal_size = 0x300;

static const ConvertPaletteType convertPaletteType = ConvertPaletteTypeNone;

typedef enum
{
    PaletteLightTypeNormal,
    PaletteLightTypeNight,
    PaletteLightTypeDay
} PaletteLightType;

static const PaletteLightType paletteLightType = PaletteLightTypeNormal;

void ConvertPalette(unsigned char &r, unsigned char &g, unsigned char &b)
{
    ConvertPalette(r, g, b, convertPaletteType);
}

void ConvertPalette(unsigned char &r, unsigned char &g, unsigned char &b, ConvertPaletteType type)
{
    switch (type)
    {
        case ConvertPaletteTypeNone:
            break;
        case ConvertPaletteTypeSepia:
        {
            int middle = (r * 30 + g * 59 + b * 11) / 100;
            r = (middle <= 255 - 46) ? middle + 46 : 255;
            g = middle + 0;
            b = (middle >= 46) ? middle - 46 : 0;
            break;
        }
        case ConvertPaletteTypeBlackAndWhite:
        {
            int middle = (r * 30 + g * 59 + b * 11) / 100;
            if (middle > 200)
            {
                middle = 255;
            }
            else if (middle > 100)
            {
                middle = 150;
            }
            else if (middle > 50)
            {
                middle = 76;
            }
            else
            {
                middle = 0;
            }
            r = middle;
            g = middle;
            b = middle;
            break;
        }
        case ConvertPaletteTypeGrayscaled:
        {
            int middle = (r * 30 + g * 59 + b * 11) / 100;
            r = middle;
            g = middle;
            b = middle;
            break;
        }
        case ConvertPaletteTypeProjection:
        {
            static const float k = 0.3f;
            int g_ = (((float)(r + b)) * (1.0f - k) + 2.0f * ((float)g) * k) * 0.5f;
            if (g_ < 0)
                g = 0;
            else if (g_ > 255)
                g = 255;
            else
                g = g_;
            break;
        }
    }
    switch (paletteLightType)
    {
        case PaletteLightTypeNormal:
            break;
        case PaletteLightTypeNight:
        {
            float k = 0.6f;
            static const int lightPorog = 230;
            static const int whitePorog = 200;
            if (r > lightPorog || g > lightPorog || b > lightPorog)
            {
                int middle = (r * 33 + g * 34 + b * 33) / 100;
                if (middle < whitePorog)
                {
                    k = 1.5f;
                }
            }
            r = (r * k >= 255) ? 255 : r * k;
            g = (g * k >= 255) ? 255 : g * k;
            b = (b * k >= 255) ? 255 : b * k;
            break;
        }
        case PaletteLightTypeDay:
        {
            float k = 1.5f;
            static const int lightPorog = 230;
            static const int whitePorog = 200;
            if (r > lightPorog || g > lightPorog || b > lightPorog)
            {
                int middle = (r * 33 + g * 34 + b * 33) / 100;
                if (middle < whitePorog)
                {
                    k = 0.8f;
                }
            }
            r = (r * k >= 255) ? 255 : r * k;
            g = (g * k >= 255) ? 255 : g * k;
            b = (b * k >= 255) ? 255 : b * k;
            break;
        }
    }
}

void ConvertPalette(Color &color)
{
    ConvertPalette(color.r, color.g, color.b);
}

MAXContentMap::MAXContentMap()
:minimap(NULL)
,map(NULL)
,mapElements(NULL)
,palette(NULL)
,groundType(NULL)
,w(0)
,h(0)
,convertPaletteType(ConvertPaletteTypeNone)
{}

MAXContentMap::~MAXContentMap()
{
    delete []minimap;
    if (map != NULL) {
        delete []map;
    }
    if (mapElements != NULL) {
        delete []mapElements;
    }
    delete []palette;
    if (groundType != NULL) {
        delete []groundType;
    }
}

void MAXContentMap::LoadShortFromBuffer(ByteBuffer &buffer)
{
    BinaryReader br(reinterpret_cast<char *>(buffer.getPointer()), buffer.getDataSize());
    {//internal name
        char namebuffer[5];
        br.ReadBuffer(5, namebuffer);
        name = string(namebuffer);
    }
    
    {//size
        w = br.ReadInt16();
        h = br.ReadInt16();
    }
    
    {//minimap
        minimap = new char[w * h];
        br.ReadBuffer(w*h, minimap);
    }
    
    {//tile indics
        br.SetPosition(br.GetPosition() + w * h * 2);
    }
    
    {//tiles
        elementCount = br.ReadInt16();
        br.SetPosition(br.GetPosition() + elementCount * 64 * 64);
    }
    {//palette
        palette = new Color[pal_size/3];
        memset(palette, 0xffffffff, pal_size);
        for (int i = 0; i < pal_size/3; i++)
        {
            unsigned char r = br.ReadChar();
            unsigned char g = br.ReadChar();
            unsigned char b = br.ReadChar();
            
            if (convertPaletteType != ConvertPaletteTypeNone) {
                if (i > 0 && i < 255) {
                    ConvertPalette(r, g, b, convertPaletteType);
                }
            }

            palette[i].r = r;
            palette[i].g = g;
            palette[i].b = b;
            palette[i].a = 255;
        }
        palette[0].a = 0;
        palette[255].a = 0;
    }
}

void MAXContentMap::LoadFromBuffer(ByteBuffer &buffer)
{
    BinaryReader br(reinterpret_cast<char *>(buffer.getPointer()), buffer.getDataSize());
    {//internal name
        char namebuffer[5]; 
        br.ReadBuffer(5, namebuffer);
        name = string(namebuffer);
    }
    
    {//size
        w = br.ReadInt16();
        h = br.ReadInt16();
    }
    
    {//minimap
        minimap = new char[w * h];
        br.ReadBuffer(w*h, minimap);
    }
    
    {//tile indics
        map = new short[w * h];
        br.ReadBuffer(w * h * 2, (char*)map);
    }
    
    {//tiles
        elementCount = br.ReadInt16();
        mapElements = new char[elementCount * 64 * 64];
        br.ReadBuffer(elementCount * 64 * 64, mapElements);
    }
    
    {//palette
        palette = new Color[pal_size/3];
        for (int i = 0; i < pal_size/3; i++)
        {
            unsigned char r = br.ReadChar();
            unsigned char g = br.ReadChar();
            unsigned char b = br.ReadChar();

            if (convertPaletteType != ConvertPaletteTypeNone) {
                if (i > 0 && i < 255) {
                    ConvertPalette(r, g, b, convertPaletteType);
                }
            }
            
            palette[i].r = r;
            palette[i].g = g;
            palette[i].b = b;
            palette[i].a = 255;
        }
        palette[0].a = 0;
        palette[255].a = 0;
    }
    
    {//ground types
        groundType = new char[elementCount];
        br.ReadBuffer(elementCount, groundType);
    }
    
}
