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
class AIMapAnalyzer;

class Map {
    
    int _w;
    int _h;
    
    MapCell*_cells;
    
    int index(int x, int y) const;
    
public:
    std::vector<GPoint2D> _landedPlayers;
    Map(const GSize2D &size);
    ~Map();

    void Random(float moreWater);
    
    MapCell &GetCell(int x, int y) { return _cells[index(x, y)]; };
    int GetW() const { return _w; };
    int GetH() const { return _h; };
    int GetMapWidth() const{ return _w; };
    int GetMapHeight() const{ return _h; };
    GRect2D GetRect() const { return GRect2D(0, 0, GetW(), GetH()); };
    GSize2D GetDeploySize();
    bool GetIsCellValid(GPoint2D cell) const;
    bool CanDeployPlayer(const GPoint2D& point, bool allowWaterDeploy, bool estimateOnly = false);
    GRect2D GetDeployRegion(GPoint2D point);
    GSize2D GetDeployMinRange(const bool ignoreEnemy);
    GPoint2D CalculateDeployPosition(GRect2D availableZone);
    GPoint2D CalculateDeployPosition(GRect2D allAvailableZone, AIMapAnalyzer* analyzer);
};


typedef struct __AIMapAnalyzerRegionInfo {
    GRect2D _rect;
    float size;
} AIMapAnalyzerRegionInfo;

typedef struct __AIMapAnalysisRectangeInfo {
    GRect2D _rect;
    float _size;

    bool _haveType;
    GPoint2D _typePoint;
} AIMapAnalysisRectangeInfo;

class AIMapAnalyzer {

    Map* _map;

public:

    std::vector<AIMapAnalyzerRegionInfo> _islands;
    std::vector<AIMapAnalyzerRegionInfo> _lakes;

    std::vector<AIMapAnalysisRectangeInfo> _waters;

    AIMapAnalyzer(Map* map);

    Map* GetMap() { return _map; }
    GSize2D GetMapSize() const;
    void AI_helper1();
    GRect2D AI_helper__fill_the_cell(int* api__map, int i, int j, const int& aval, const char aterr_type, char* apc_flag, int& a_ret_area);

    AIMapAnalysisRectangeInfo testRectangle(const GRect2D& rect, bool checkCost, const GPoint2D landPoint, Map_Cell_Type needType);
    std::vector<AIMapAnalysisRectangeInfo> testWater(const GRect2D& current);
    std::vector<AIMapAnalysisRectangeInfo> testGround(const GRect2D& current);
};


#endif
