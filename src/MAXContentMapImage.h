#ifndef __MAXContentMapImage__
#define __MAXContentMapImage__

#include "BitmapTexture.h"


class MAXContentMap;

class MAXContentMapImage: public BitmapTexture {
    
public:
    MAXContentMapImage(const MAXContentMap &map);
    MAXContentMapImage(const MAXContentMapImage &map, int newW, int newH);
    
};


#endif
