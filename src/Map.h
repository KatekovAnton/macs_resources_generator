#ifndef Map_h
#define Map_h

#include "Geometry.h"



typedef enum __Map_Cell_Type
{
    Map_Cell_Ground,
    Map_Cell_Water,
} Map_Cell_Type;

typedef enum __Map_Cell_Res_Type
{
    Map_Cell_Res_Type_Raw,
    Map_Cell_Res_Type_Fuel,
    Map_Cell_Res_Type_Gold,
} Map_Cell_Res_Type;

struct MapCell
{
    Map_Cell_Type _type = Map_Cell_Ground;
    Map_Cell_Res_Type _resType = Map_Cell_Res_Type_Raw;
    int resourcesAmount = 0;
};


class Map {
    
    int _w;
    int _h;
    
    MapCell*_cells;
    
    int index(int x, int y) const;
    
public:
    
    Map(const GSize2D &size);
    ~Map();

    void Random(float moreWater);
    
    MapCell &GetCell(int x, int y) { return _cells[index(x, y)]; };
    int GetW() const { return _w; };
    int GetH() const { return _h; };
    GSize2D GetDeploySize();
    GRect2D GetDeployRegion(GPoint2D point);
};

#endif
