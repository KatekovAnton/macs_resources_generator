#include "MAXContentMapImage.h"
#include "MAXContentMap.h"



MAXContentMapImage::MAXContentMapImage(const MAXContentMap &map)
:BitmapTexture(GSize2D(map.w * 64, map.h * 64))
{
    for (int xc = 0; xc < map.w * 64; xc ++) {
        for (int yc = 0; yc < map.h * 64; yc ++) {
     
            int blockX = xc / 64;
            int blockY = yc / 64;
            
            int blockPixelX = xc % 64;
            int blockPixelY = yc % 64;
            
            int tileIndex = static_cast<int>(map.map[blockY * map.w + blockX]);
            int colorIndex = map.mapElements[tileIndex * 64 * 64 + blockPixelY * 64 + blockPixelX];
            Color pixelColor = map.palette[colorIndex];
            setColor(pixelColor, xc, yc);
        }
    }
}

MAXContentMapImage::MAXContentMapImage(const MAXContentMapImage &map, int newW, int newH)
:BitmapTexture(GSize2D(newW, newH))
{
    float scaleX = (float)newW / map.getSize().width;
    float scaleY = (float)newH / map.getSize().height;
    for (int xc = 0; xc < newW; xc ++) {
        for (int yc = 0; yc < newH; yc ++) {
            float x = xc / scaleX;
            float y = yc / scaleY;
            Color pixelColor = map.getColor(x, y); 
            setColor(pixelColor, xc, yc);
        }
    }
}
