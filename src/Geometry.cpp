#include "Geometry.h"
#include "math.h"



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

