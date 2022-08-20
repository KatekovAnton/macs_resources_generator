#include "Geometry.h"
#include "math.h"
#include <algorithm>



bool __GRect2D::ContainsPoint(const GPoint2D &point) const {
    return point.x > origin.x &&
    point.y > origin.y &&
    point.x < origin.x + size.width &&
    point.y < origin.y + size.height;
}




__GRect2D::__GRect2D(const __BoundingBox &bb)
:origin(bb.min.x, bb.min.y)
,size(bb.max.x - bb.min.x, bb.max.y - bb.min.y)
{
    
}

GRect2D RectWithCenterAndRadius(GPoint2D center, float radius)
{
    GRect2D result;

    result.origin.x = center.x - radius;
    result.origin.y = center.y - radius;

    result.size.width = radius * 2;
    result.size.height = radius * 2;

    return result;
}

std::vector<GPoint2D> GUtils::CalculateLine(const GPoint2D &from, const GPoint2D &to)
{
    std::vector<GPoint2D> result;
    
    float x1 = from.x;
    float y1 = from.y;
    float x2 = to.x;
    float y2 = to.y;
    
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    
    if(x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);
    
    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;
    
    const int maxX = (int)x2;
    
    for(int x=(int)x1; x<maxX; x++)
    {
        if(steep) {
            result.push_back(GPoint2D(y,x));
        }
        else {
            result.push_back(GPoint2D(x,y));
        }
        
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
    
    return result;
}

GRect2D GUtils::ClampRectToOuterRect(const GRect2D &rect, const GRect2D &outerRect)
{
    BoundingBox bba(outerRect);
    BoundingBox bb(rect);
    if (bb.min.x < bba.min.x)
        bb.min.x = bba.min.x;
    if (bb.min.y < bba.min.y)
        bb.min.y = bba.min.y;
    if (bb.max.x >= bba.max.x)
        bb.max.x = bba.max.x;
    if (bb.max.y >= bba.max.y)
        bb.max.y = bba.max.y;
    return GRect2D(bb);
}

GRect2D AIUtils::ExpandRect(const GRect2D& rect, float expand)
{
    return GRect2D(rect.origin.x - expand / 2, rect.origin.y - expand / 2, rect.size.width + expand, rect.size.height + expand);
}

std::vector<GPoint2D> AIUtils::GetPossiblePointsAroundPoint(const GPoint2D& point, const GRect2D& inRect, std::function<bool(const GPoint2D&)> checFunction, int maximumNumber, int checkCount)
{
    GPoint2D directions[] = {
        GPoint2D(-1, 0),
        GPoint2D(0, 1),
        GPoint2D(1, 0),
        GPoint2D(0, -1) };

    GPoint2D corners[] = { GPoint2D(point.x - 1, point.y - 1),
        GPoint2D(point.x - 1, point.y + 1),
        GPoint2D(point.x + 1, point.y + 1),
        GPoint2D(point.x + 1, point.y - 1) };

    int direction = 0;
    int corner = 0;
    std::vector<GPoint2D> result;
    GPoint2D newPoint = corners[3];

    int step = 0;
    while (step < checkCount)
    {
        step++;

        if (GRect2DContainsPointInt(inRect, newPoint))
        {
            bool vaild = checFunction(newPoint);
            if (vaild)
            {
                result.push_back(newPoint);
                if (result.size() >= maximumNumber) {
                    break;
                }
            }
        }

        newPoint.x += directions[direction].x;
        newPoint.y += directions[direction].y;

        if ((int)newPoint.x == (int)corners[corner].x &&
            (int)newPoint.y == (int)corners[corner].y)
        {
            direction++;
            if (direction == 4)
            {
                direction = 0;
                corners[0].x -= 1;
                corners[0].y -= 1;

                corners[1].x -= 1;
                corners[1].y += 1;

                corners[2].x += 1;
                corners[2].y += 1;

                corners[3].x += 1;
                corners[3].y -= 1;

                newPoint = corners[3];
            }
            corner++;
            if (corner == 4) {
                corner = 0;
            }
        }
    }
    return result;
}

GRect2D AIUtils::ClampRectToArea(const GRect2D& rect, const GRect2D& area)
{
    BoundingBox bba = BoundingBoxMake(area);
    BoundingBox bb = BoundingBoxMake(rect);
    if (bb.min.x < bba.min.x)
        bb.min.x = bba.min.x;
    if (bb.min.y < bba.min.y)
        bb.min.y = bba.min.y;
    if (bb.max.x >= bba.max.x)
        bb.max.x = bba.max.x;
    if (bb.max.y >= bba.max.y)
        bb.max.y = bba.max.y;

    return RectFromBoundingBox(bb);
}

GRect2D AIUtils::RectFromBoundingBox(const BoundingBox& bb)
{
    return GRect2D(bb.min.x, bb.min.y, bb.max.x - bb.min.x, bb.max.y - bb.min.y);
}


float AIUtils::GetIntersectionArea(const BoundingBox& f, const BoundingBox& s)
{
    std::vector<float> X(4);
    std::vector<float> Y(4);

    X[0] = f.min.x; Y[0] = f.min.y;
    X[1] = f.max.x; Y[1] = f.max.y;

    X[2] = s.min.x; Y[2] = s.min.y;
    X[3] = s.max.x; Y[3] = s.max.y;


    if (X[0] >= X[1] || Y[0] >= Y[1] || X[2] >= X[3] || Y[2] >= Y[3]) { // bad rect
        return 0;
    }
    else if (X[0] >= X[3] || X[2] >= X[1] || Y[3] <= Y[0] || Y[1] <= Y[2]) { // not intersects
        return 0;
    }

    std::sort(X.begin(), X.end());
    std::sort(Y.begin(), Y.end());

    return (X[2] - X[1]) * (Y[2] - Y[1]);
}

float AIUtils::GetIntersectionArea(const GRect2D& first, const GRect2D& second)
{
    BoundingBox f = BoundingBoxMake(first);
    BoundingBox s = BoundingBoxMake(second);

    return GetIntersectionArea(f, s);
}
