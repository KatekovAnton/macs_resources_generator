#include "Map.h"
#include <iostream>
#include "NoiseGenerator.h"



Map::Map(const GSize2D &size)
:_w(size.width)
,_h(size.height)
{
    _cells = (Map_Cell_Type *)malloc(_w * _h * sizeof(Map_Cell_Type));
    
    
}

Map::~Map()
{
    free(_cells);
}
