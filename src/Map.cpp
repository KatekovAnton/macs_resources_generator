#include "Map.h"
#include <iostream>
#include <math.h>
#include "NoiseGenerator.h"



using namespace std;



Map::Map(const GSize2D &size)
:_w(size.width)
,_h(size.height)
{
    _cells = (MapCell*)malloc(_w * _h * sizeof(MapCell));

}

int Map::index(int x, int y) const
{
    return y * _w + x;
}

void Map::Random(float moreWater)
{
    PerlinNoise p(rand());
    for (int x = 0; x < _w; x++) {
        for (int y = 0; y < _h; y++) {
            float n = p.noise((float)x / 30, (float)y / 30, 1);
            n -= moreWater;
            Color color;
            if (n > 0.5) {
                _cells[index(x, y)]._type = Map_Cell_Ground;
            }
            else {
                _cells[index(x, y)]._type = Map_Cell_Water;
            }
        }
    }
}

GSize2D Map::GetDeploySize()
{
    const GSize2D mapSize = GSize2D(_w, _h);
    const bool isAI = false;// player->GetIsAI();
    const int teamCount = 2;// GetNumberOfAliveTeams();
    const bool manyTeams = (teamCount > 3);
    const bool hugeTeams = (teamCount > 8);
    const int minSize = (hugeTeams
        ? (isAI ? 12 : 9)
        : (manyTeams
            ? (isAI ? 16 : 11)
            : (isAI ? 21 : 17)));
    float k = (hugeTeams
        ? (isAI ? 0.2f : 0.15f)
        : (manyTeams
            ? (isAI ? 0.3f : 0.2f)
            : (isAI ? 0.4f : 0.3f)));
    // if (teamCount == 2 && GetSettings()->_startGold > 1000 && !isAI) {
    //     k *= 1.5f;
    // }
    int areaSize = max(minSize, int((mapSize.getW() + mapSize.getH()) * 0.5f * k));
    return GSize2D(areaSize, areaSize);
}

GRect2D Map::GetDeployRegion(GPoint2D point)
{
    GSize2D size = GetDeploySize();
    GSize2D halfSize = GSize2D(int(size.getW() * 0.5f), int(size.getH() * 0.5f));
    GRect2D result = GRect2D(max(0, (int)(point.x - halfSize.getW())),
        max(0, (int)(point.y - halfSize.getH())),
        size.getW(),
        size.getH());

    GSize2D mapSize = GSize2D(_w, _h);
    if (result.getX() > mapSize.getW() - result.getW())
    {
        result.setX(mapSize.getW() - result.getW());
    }
    if (result.getY() > mapSize.getH() - result.getH())
    {
        result.setY(mapSize.getH() - result.getH());
    }
    return result;
}

Map::~Map()
{
    free(_cells);
}
