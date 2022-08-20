#include "BitmapTexture.h"
#include <stdio.h>
#include <iostream>
#include <png.h>
#include <cstring>
#include <string.h>



BitmapTexture::BitmapTexture(const GSize2D &size)
:_size(size)
{
    size_t sizeb = sizeof(Color) * size.width * size.height;
    _buffer = (Color *) malloc(sizeb);
    memset(_buffer, 0xffffffff, sizeb);
}

BitmapTexture::~BitmapTexture()
{
    free(_buffer);
}

void BitmapTexture::setColor(Color color, int x, int y)
{
    _buffer[y * (int)_size.width + x] = color;
}

void BitmapTexture::setColor(float color, int x, int y)
{
    setColor(Color((unsigned char)(color * 255.0f), (unsigned char)(color * 255.0f), (unsigned char)(color * 255.0f), 255), x, y);
}

GSize2D BitmapTexture::getSize()
{
    return _size;
}

Color *BitmapTexture::getColorBuffer()
{
    return _buffer;
}

void BitmapTexture::DrawLine(const GPoint2D &from, const GPoint2D &to, const Color &color)
{
    auto line = GUtils::CalculateLine(from, to);
    GRect2D bounds(0, 0, _size.width, _size.height);
    for (int i = 0; i < line.size(); i++) {
        if (bounds.ContainsPoint(line[i])) {
            setColor(color, line[i].x, line[i].y);
        }
    }
}

void BitmapTexture::DrawRect(const GRect2D& rect, const Color& color)
{
    DrawLine(
        GPoint2D(rect.getX(), rect.getY()), 
        GPoint2D(rect.getX(), rect.getY() + rect.getH()), 
        color);
    DrawLine(
        GPoint2D(rect.getX(), rect.getY() + rect.getH()), 
        GPoint2D(rect.getX() + rect.getW(), rect.getY() + rect.getH()), 
        color);
    DrawLine(
        GPoint2D(rect.getX() + rect.getW(), rect.getY() + rect.getH()), 
        GPoint2D(rect.getX() + rect.getW(), rect.getY()),
        color);
    DrawLine(
        GPoint2D(rect.getX() + rect.getW(), rect.getY()),
        GPoint2D(rect.getX(), rect.getY()), 
        color);
}

inline void setRGB(png_byte *ptr, const Color &c)
{
    ptr[0] = c.r; ptr[1] = c.g; ptr[2] = c.b;
    
}
void BitmapTexture::Save(const std::string &filename, const std::string &title)
{
    
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    
    // Open file for writing (binary mode)
    fp = fopen(filename.c_str(), "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename.c_str());
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, (int)_size.width, (int)_size.height,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Set title
    {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = const_cast<char *>(title.c_str());
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (3 bytes per pixel - RGB)
    row = (png_bytep) malloc(3 * (int)_size.width * sizeof(png_byte));

    // Write image data
    int x, y;
    for (y=0 ; y<(int)_size.height ; y++) {
        for (x=0 ; x<(int)_size.width ; x++) {
            setRGB(&(row[x*3]), _buffer[y*(int)_size.width + x]);
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

    finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);
}
