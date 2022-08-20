#ifndef Map_h
#define Map_h

#include "Geometry.h"



typedef enum __Map_Cell_Type
{
    Map_Cell_Ground,
    Map_Cell_Water,
} Map_Cell_Type;



class Map {
    
    int _w;
    int _h;
    
    Map_Cell_Type *_cells;
    
    int index(int x, int y) const;
    
public:
    
    Map(const GSize2D &size);
    ~Map();
    
    Map_Cell_Type GetCellType(int x, int y) { return _cells[index(x, y)]; };
};

#endif
