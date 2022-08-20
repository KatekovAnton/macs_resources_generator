#ifndef BitmapTexture_h
#define BitmapTexture_h

#include <stdio.h>
#include <string>
#include "Geometry.h"

class BitmapTexture {
    
    GSize2D _size;
    Color *_buffer;
    
public:
    
    BitmapTexture(const GSize2D &size);
    ~BitmapTexture();
    
    void setColor(Color color, int x, int y);
    void setColor(float color, int x, int y);
    GSize2D getSize();
    Color *getColorBuffer();
    
    void DrawLine(const GPoint2D &from, const GPoint2D &to, const Color &color);
    
    void Save(const std::string &filename, const std::string &title);
};

#endif
