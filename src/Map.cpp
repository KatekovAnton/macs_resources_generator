#include "Map.h"
#include <iostream>
#include <functional>
#include <math.h>
#include "NoiseGenerator.h"



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
    int areaSize = std::max(minSize, int((mapSize.getW() + mapSize.getH()) * 0.5f * k));
    return GSize2D(areaSize, areaSize);
}

GRect2D Map::GetDeployRegion(GPoint2D point)
{
    GSize2D size = GetDeploySize();
    GSize2D halfSize = GSize2D(int(size.getW() * 0.5f), int(size.getH() * 0.5f));
    GRect2D result = GRect2D(std::max(0, (int)(point.x - halfSize.getW())),
        std::max(0, (int)(point.y - halfSize.getH())),
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

static float nextFloat()
{
    return (float)(rand() % 1000) / 1000.0;
}

static int nextInt()
{
    return rand();
}

static int nextIntMax(int max)
{
    return rand() % max;
}

static int nextIntMinMax(int min, int max)
{
    int range = max - min;
    int value = nextIntMax(range);
    return value + min;
}

GSize2D Map::GetDeployMinRange(const bool ignoreEnemy)
{

    bool isEnemy = true;//PlayerIsEnemyToPlayer(player1, player2);
    if (isEnemy && ignoreEnemy) {
        return GSize2D(-1, -1);
    }
    GSize2D size1 = GetDeploySize();
    GSize2D size2 = GetDeploySize();
    const bool manyPlayers = false;// (_playersCount > 3); // May be _teamCounts?
    const bool hugePlayers = false;// (_playersCount > 8);
    // Part of half zones which set zones distance.
    // 0.5 means exactly two zone can be placed close to each other
    // < 0.5 means zones can overlap each other. Applicable for ally players
    // > 0.5 means some free territiry exist between each zone
    const float k = (isEnemy
        ? (hugePlayers
            ? 0.52f
            : (manyPlayers
                ? 0.57f
                : 0.65f))
        : 0.2f);
    GSize2D halfSize1 = GSize2D(int(size1.getW() * k + 0.5f),
        int(size1.getH() * k + 0.5f));
    GSize2D halfSize2 = GSize2D(int(size2.getW() * k + 0.5f),
        int(size2.getH() * k + 0.5f));
    return GSize2D(halfSize1.getW() + halfSize2.getW(),
        halfSize1.getH() + halfSize2.getH());

}

bool Map::GetIsCellValid(GPoint2D cell) const
{
    return cell.x >= 0 && cell.y >= 0 && cell.x < _w&& cell.y < _h;
}

std::vector<GPoint2D> GetBaseDeployPoints(GPoint2D startPos, bool radar, bool base)
{
    std::vector<GPoint2D>result;
    if (!base) {
        return result;
    }
    result.push_back(GPoint2D(startPos.x - 1, startPos.y + 1)); // 0 powergen
    if (radar) {
        result.push_back(GPoint2D(startPos.x - 1, startPos.y)); // 1 radar
    }
    result.push_back(startPos); // 2 mining

    return result;
}

bool Map::CanDeployPlayer(const GPoint2D& point, bool allowWaterDeploy, bool estimateOnly)
{
    // Check landed players regions
    for (int i = 0; i < _landedPlayers.size(); i++)
    {
        GPoint2D landingPosition = _landedPlayers[i];
        auto region1 = GetDeployRegion(point);
        auto region2 = GetDeployRegion(landingPosition);
        if (AIUtils::GetIntersectionArea(region1, region2) >= 1) {
            return false;
        }
    }

    //1st powergen, mining and radar should be
    //inside of map borders and on the ground if allowWaterDeploy not allowed
    std::vector<GPoint2D> points = GetBaseDeployPoints(point, true, true);
  
    for (int i = 0; i < points.size(); i++)
    {
        GPoint2D p = points[i];
        if (!GetIsCellValid(p)) {
            return false;
        }
        if (GetCell(point.x, point.y)._type != Map_Cell_Ground) {
            return false;
        }
    }
    return true;
}

GPoint2D Map::CalculateDeployPosition(GRect2D allAvailableZone, AIMapAnalyzer* analyzer)
{
    GRect2D availableZone = AIUtils::ExpandRect(allAvailableZone, -1);
    std::vector<AIMapAnalyzerRegionInfo> islands;
    islands = analyzer->_islands;


    std::vector<AIMapAnalyzerRegionInfo> suitableIslands;
    for (int i = 0; i < islands.size(); i++)
    {
        if (islands[i].size > 90) {
            suitableIslands.push_back(islands[i]);
        }
    }

    float totalMaxRangeOfMinRanges = 0;
    GPoint2D totalMaxDistancedPoint;
    int attempts = 10;
    for (int attempt = 0; attempt < attempts; attempt++) {
        int island = nextIntMax(static_cast<int>(suitableIslands.size()));
        GRect2D islandToDeploy = suitableIslands[island]._rect;

        // pass this zone to script and return point to deploy
        // temporary - center of zone
        GPoint2D point = GPoint2D(islandToDeploy.origin.x + islandToDeploy.size.width * nextFloat(),
            islandToDeploy.origin.y + islandToDeploy.size.height * nextFloat());

        std::function<bool(const GPoint2D&)> a = [this](const GPoint2D& p)->bool {
            return this->CanDeployPlayer(p, false);
        };

        auto points = AIUtils::GetPossiblePointsAroundPoint(point, availableZone, a, 1, availableZone.getW() * availableZone.getH());

        if (points.size() > 0) {
            point = points[0];
        }
        else {
            continue;
        }

        {
            float minRange = 9999;
            for (int i = 0; i < _landedPlayers.size(); i++) {
                GPoint2D cell = _landedPlayers[i];
                float range = GPoint2DLength(GPoint2DSub(point, cell));
                if (range < minRange) {
                    minRange = range;
                }
            }

            if (minRange > totalMaxRangeOfMinRanges || attempt == 0) {
                totalMaxRangeOfMinRanges = minRange;
                totalMaxDistancedPoint = point;
            }
        }
    }

    return totalMaxDistancedPoint;
}




AIMapAnalyzer::AIMapAnalyzer(Map *map)
    :_map(map)
{
    _waters = testWater(GRect2D((int)(map->GetW() / 3),
        (int)(map->GetH() / 3),
        (int)(map->GetW() / 3),
        (int)(map->GetH() / 3)));
}

GSize2D AIMapAnalyzer::GetMapSize() const
{
    return GSize2D(_map->GetW(), _map->GetH());
}

void AIMapAnalyzer::AI_helper1()
{
    _islands.clear();
    int ta[] = { 0,0,0,0,0 }; //temp;

    int _w = _map->GetW();
    int _h = _map->GetH();

#if 1 //move me to class; to save map info;
    int parts_for_1_player_w = (int)((_w / 32.0f) + 0.5f);
    int parts_for_1_player_h = (int)((_h / 32.0f) + 0.5f);

    int sz_for_1_player_w = _w / (parts_for_1_player_w ? parts_for_1_player_w : 1);
    int sz_for_1_player_h = _h / (parts_for_1_player_h ? parts_for_1_player_h : 1);

    int islands_id_map_ground_Q = 0;
    int islands_id_map_water__Q = 0;


   // const char* const _groundType = _map->GetGroundType();
    int* islands_id_map_ground = (int*)malloc(_w * _h * sizeof(int));
    int* islands_id_map_water_ = (int*)malloc(_w * _h * sizeof(int));
    char* bm_flag = (char*)malloc(_w * _h * sizeof(char)); // 0-ground 1-water 2-others;  +10 = unique field, island start;
#endif

#if 1 //setup the arrays;
    for (int i = 0; i < _h; i++)
    {
        for (int j = 0; j < _w; j++)
        {

            islands_id_map_ground[i * _w + j] = 0;
            islands_id_map_water_[i * _w + j] = 0;
            bm_flag[i * _w + j] = 3;

            int element = _map->GetCell(i, j)._type;

            if (0 == element)
            {
                bm_flag[i * _w + j] = 0;
                islands_id_map_ground[i * _w + j] = i * _w + j + 1;
            }
            else
                if (1 == element)
                {
                    bm_flag[i * _w + j] = 1;
                    islands_id_map_water_[i * _w + j] = i * _w + j + 1;
                }
                else
                {
                    element = 4;
                }

            ta[element] ++;  //0-ground 1-water 2-coast 3-unpassable
        }
    }
#endif

#if 1 //fill the arrays to find islands;
    for (int i = 0; i < _h; i++)
    {
        for (int j = 0; j < _w; j++)
        {
            if (0 == bm_flag[i * _w + j])
            {
                islands_id_map_ground_Q++;
                char aterr_type = bm_flag[i * _w + j];
                bm_flag[i * _w + j] += 10;
                int a_ret_area = 0;
                GRect2D rect = AI_helper__fill_the_cell(islands_id_map_ground, i, j, islands_id_map_ground[i * _w + j], aterr_type, bm_flag, a_ret_area);
                AIMapAnalyzerRegionInfo info;
                info._rect = rect;
                info.size = a_ret_area;
                _islands.push_back(info);

               // ULog("Island start: X:%3d Y:%3d W:%3d H:%3d, area=%6d, map_ratio=%f", (int)rect.origin.x, (int)rect.origin.y, (int)rect.size.width, (int)rect.size.height, a_ret_area, 1.0f * a_ret_area / _w / _h);
            }
            else
            {
                if (1 == bm_flag[i * _w + j])
                {
                    islands_id_map_water__Q++;
                    char aterr_type = bm_flag[i * _w + j];
                    bm_flag[i * _w + j] += 10;
                    int a_ret_area = 0;
                    GRect2D rect = AI_helper__fill_the_cell(islands_id_map_water_, i, j, islands_id_map_water_[i * _w + j], aterr_type, bm_flag, a_ret_area);
                    AIMapAnalyzerRegionInfo info;
                    info._rect = rect;
                    info.size = a_ret_area;
                    _lakes.push_back(info);
                  //  ULog("Lake   start: %3d , %3d, area=%6d, map_ratio=%f", i, j, a_ret_area, 1.0f * a_ret_area / _w / _h);
                }
            }
        }
    }

   // ULog("Lakes q: %d , islands q: %d   map size: %dx%d=%d", islands_id_map_water__Q, islands_id_map_ground_Q, _w, _h, _w * _h);
#endif


    //todo: move to destructor;
    free(islands_id_map_ground);
    free(islands_id_map_water_);
    free(bm_flag);
}

//debug: stack->a_ret_area:   1M-13231   2M-22414   4M-34961   8M-50179 passed (224*224)
GRect2D AIMapAnalyzer::AI_helper__fill_the_cell(int* api__map, int i, int j, const int& aval, const char aterr_type, char* apc_flag, int& a_ret_area)
{
    BoundingBox bb;
    bb.min = GPoint2D(j, i);
    bb.max = GPoint2D(j, i);

    char* ca_directions_stack = (char*)malloc(1024 * 1024);
    int ca_directions_stack_sz = 0;


    int _w = _map->GetW();
    int _h = _map->GetH();


    a_ret_area++;
    api__map[i * _w + j] = aval;
    if (apc_flag[i * _w + j] < 9) // start of some island;
        apc_flag[i * _w + j] = 2; // marked;

    while (0 <= ca_directions_stack_sz)
    {
        char c_new_direction = 0;

        if (j + 1 < _w)
        {
            if (apc_flag[(i + 0) * _w + (j + 1)] == aterr_type) // unmarked == unvisited;
            {
                c_new_direction = 1;	goto m_new_plot;
            }

            if (0 <= i - 1)
                if (apc_flag[(i - 1) * _w + (j + 1)] == aterr_type) // unmarked == unvisited;
                {
                    c_new_direction = 2;	goto m_new_plot;
                }

            if (i + 1 < _h)
                if (apc_flag[(i + 1) * _w + (j + 1)] == aterr_type) // unmarked == unvisited;
                {
                    c_new_direction = 3;	goto m_new_plot;
                }
        }

        if (0 <= j - 1)
        {
            if (apc_flag[(i + 0) * _w + (j - 1)] == aterr_type) // unmarked == unvisited;
            {
                c_new_direction = 4;	goto m_new_plot;
            }

            if (0 <= i - 1)
                if (apc_flag[(i - 1) * _w + (j - 1)] == aterr_type) // unmarked == unvisited;
                {
                    c_new_direction = 5;	goto m_new_plot;
                }

            if (i + 1 < _h)
                if (apc_flag[(i + 1) * _w + (j - 1)] == aterr_type) // unmarked == unvisited;
                {
                    c_new_direction = 6;	goto m_new_plot;
                }
        }

        if (0 <= i - 1)
            if (apc_flag[(i - 1) * _w + (j + 0)] == aterr_type) // unmarked == unvisited;
            {
                c_new_direction = 7;	goto m_new_plot;
            }

        if (i + 1 < _h)
            if (apc_flag[(i + 1) * _w + (j + 0)] == aterr_type) // unmarked == unvisited;
            {
                c_new_direction = 8;	goto m_new_plot;
            }

        if (c_new_direction)
        {
        m_new_plot:;
            if (1 == c_new_direction) { i;	j++; }
            else if (2 == c_new_direction) { i--;	j++; }
            else if (3 == c_new_direction) { i++;	j++; }
            else if (4 == c_new_direction) { i;	j--; }
            else if (5 == c_new_direction) { i--;	j--; }
            else if (6 == c_new_direction) { i++;	j--; }
            else if (7 == c_new_direction) { i--;	j; }
            else if (8 == c_new_direction) { i++;	j; }
            else;
            a_ret_area++;
            ca_directions_stack[ca_directions_stack_sz] = c_new_direction;
            ca_directions_stack_sz++;

            if (j < bb.min.x) {
                bb.min.x = j;
            }
            if (j > bb.max.x) {
                bb.max.x = j;
            }
            if (i < bb.min.y) {
                bb.min.y = i;
            }
            if (i > bb.max.y) {
                bb.max.y = i;
            }

            api__map[i * _w + j] = aval;
            if (apc_flag[i * _w + j] < 9) // start of some island;
                apc_flag[i * _w + j] = 2; // marked;
        }
        else
        {
            ca_directions_stack_sz--;
            if (0 <= ca_directions_stack_sz)
            {
                c_new_direction = ca_directions_stack[ca_directions_stack_sz];
                if (c_new_direction)
                {
                    if (1 == c_new_direction) { i;	j--; }
                    else if (2 == c_new_direction) { i++;	j--; }
                    else if (3 == c_new_direction) { i--;	j--; }
                    else if (4 == c_new_direction) { i;	j++; }
                    else if (5 == c_new_direction) { i++;	j++; }
                    else if (6 == c_new_direction) { i--;	j++; }
                    else if (7 == c_new_direction) { i++;	j; }
                    else if (8 == c_new_direction) { i--;	j; }
                    else;
                }
            }
        }
    }

    free(ca_directions_stack);
    return GRect2D(bb.min.x, bb.min.y, bb.max.x - bb.min.x, bb.max.y - bb.min.y);
}

AIMapAnalysisRectangeInfo AIMapAnalyzer::testRectangle(const GRect2D& rect, bool checkCost, const GPoint2D landPoint, Map_Cell_Type needType)
{
    AIMapAnalysisRectangeInfo result;
    result._rect = rect;
    result._size = rect.getW() * rect.getH();
    result._haveType = false;

    auto map = _map;
    std::function<bool(const GPoint2D&)> a = [map, checkCost, landPoint, needType](const GPoint2D& p)->bool {

        if (map->GetCell(p.x, p.y)._type != needType) {
            return false;
        }

        GRect2D internal = RectWithCenterAndRadius(p, 3);
        internal = AIUtils::ClampRectToArea(internal, map->GetRect());

        std::function<bool(const GPoint2D&)> ia = [map, needType](const GPoint2D& p)->bool {
            return map->GetCell(p.x, p.y)._type == needType;
        };
        auto points = AIUtils::GetPossiblePointsAroundPoint(p, internal, ia, internal.getW() * internal.getH(), internal.getW() * internal.getH());
        if (points.size() < 16) {
            return false;
        }

        GPoint2D point = GPoint2DAdd(p, GPoint2DMult(GPoint2DSub(p, landPoint), 5));
        auto line = GUtils::CalculateLine(p, point);
        bool enoughWater = false;
        for (int i = 0; i < line.size(); i++) {
            if (!map->GetIsCellValid(line[i])) {
                continue;;
            }
            if (map->GetCell(line[i].x, line[i].y)._type != needType) {
                break;
            }
            if (i >= 5) {
                enoughWater = true;
                break;
            }
        }

        return enoughWater;
    };
    auto w = AIUtils::GetPossiblePointsAroundPoint(rect.GetCenter(), rect, a, 1, result._size);
    result._haveType = w.size() == 1;
    if (result._haveType) {
        result._typePoint = w[0];
    }
    return result;
}

std::vector<AIMapAnalysisRectangeInfo> AIMapAnalyzer::testWater(const GRect2D& current)
{
    std::vector<AIMapAnalysisRectangeInfo> result;
    GRect2D rect = GRect2D((int)current.getX(), (int)current.getY(), (int)current.getW(), (int)current.getH());
    auto r = testRectangle(rect, true, current.GetCenter(), Map_Cell_Water);
    if (!r._haveType) {
        return result;
    }
    result.push_back(r);

    for (int i = 0; i < 40; i++) {
        GRect2D frame = RectWithCenterAndRadius(GPoint2D(nextIntMax(_map->GetMapWidth() - 7) + 3, nextIntMax(_map->GetMapHeight() - 7) + 3), 3);
        int intersection = AIUtils::GetIntersectionArea(frame, current);
        int attempts = 0;
        int maxAttempts = 20;

        while (intersection > 1) {
            GRect2D newFrame = RectWithCenterAndRadius(GPoint2D(nextIntMax(_map->GetMapWidth() - 7) + 3, nextIntMax(_map->GetMapHeight() - 7) + 3), 3);
            int newIntersection = AIUtils::GetIntersectionArea(frame, current);
            if (newIntersection < intersection) {
                frame = newFrame;
                intersection = newIntersection;
            }
            attempts++;
            if (attempts >= maxAttempts) {
                break;
            }
        }

        r = testRectangle(frame, false, GPoint2D(), Map_Cell_Water);
        if (r._haveType) {
            result.push_back(r);
        }
        if (result.size() >= 5) {
            break;;
        }
    }

    return result;
}

std::vector<AIMapAnalysisRectangeInfo> AIMapAnalyzer::testGround(const GRect2D& current)
{
    std::vector<AIMapAnalysisRectangeInfo> result;
    GRect2D rect = GRect2D((int)current.getX(), (int)current.getY(), (int)current.getW(), (int)current.getH());
    auto r = testRectangle(rect, true, current.GetCenter(), Map_Cell_Ground);
    if (!r._haveType) {
        return result;
    }
    result.push_back(r);

    for (int i = 0; i < 40; i++) {
        GRect2D frame = RectWithCenterAndRadius(GPoint2D(nextIntMax(_map->GetMapWidth() - 7) + 3, nextIntMax(_map->GetMapHeight() - 7) + 3), 3);
        int intersection = AIUtils::GetIntersectionArea(frame, current);
        int attempts = 0;
        int maxAttempts = 20;

        while (intersection > 1) {
            GRect2D newFrame = RectWithCenterAndRadius(GPoint2D(nextIntMax(_map->GetMapWidth() - 7) + 3, nextIntMax(_map->GetMapHeight() - 7) + 3), 3);
            int newIntersection = AIUtils::GetIntersectionArea(frame, current);
            if (newIntersection < intersection) {
                frame = newFrame;
                intersection = newIntersection;
            }
            attempts++;
            if (attempts >= maxAttempts) {
                break;
            }
        }

        r = testRectangle(frame, false, GPoint2D(), Map_Cell_Ground);
        if (r._haveType) {
            result.push_back(r);
        }
        if (result.size() >= 5) {
            break;;
        }
    }

    return result;
}

