#ifndef Geometry_h
#define Geometry_h

#include <vector>
#include <functional>
#include "math.h"



typedef struct __BoundingBox BoundingBox;
#define __inline__ inline

template <typename T> static __inline__
T ____min(T a, T b)
{
    return a<b?a:b;
}

template <typename T> static __inline__
T ____max(T a, T b)
{
    return a>b?a:b;
}

template <typename T> static __inline__
T ____abs(T a)
{
    return a>0?a:-a;
}

template <typename T> static __inline__
T ____lerp(T from, T to, float thetta)
{
    return from + (to - from) * thetta;
}

struct __GPoint2D {
    float x;
    float y;
    
    __GPoint2D() :x(0), y(0) {}
    __GPoint2D(float _x, float _y) :x(_x), y(_y) {}
    
    float getX() const {return x;}
    float getY() const {return y;}
    
    void setX(float value) {x = value;}
    void setY(float value) {y = value;}
};

typedef struct __GPoint2D GPoint2D;

typedef struct __Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    
    __Color ()
    :r(0), g(0), b(0), a(0)
    {}
    
    __Color (unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
    :r(_r), g(_g), b(_b), a(_a)
    {}
    
    inline bool operator == (const __Color &color) const
    {
        return color.r == r && color.b == b && color.g == g && color.a == a; //IsNear(r, color.r) && IsNear(g, color.g) && IsNear(b, color.b) && IsNear(a, color.a);
    }
    
} Color;

struct __ColorRGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    
    bool IsNear(unsigned char a1, unsigned char a2) const
    {
        return ____max(a1, a2) - ____min(a1, a2)<3;
    }
    
    inline bool operator == (const __Color &color) const
    {
        return color.r == r && color.b == b && color.g == g;
    }
    
} typedef ColorRGB;

struct __ColorAlpha
{
    unsigned char a;
    
    bool IsNear(unsigned char a1, unsigned char a2) const
    {
        return ____max(a1, a2) - ____min(a1, a2)<3;
    }
    
    inline bool operator == (const __Color &color) const
    {
        return color.a == a;
    }
    
} typedef ColorAlpha;



static __inline__ GPoint2D
GPoint2DMult(const GPoint2D& v, const float s)
{
    return GPoint2D(v.x * s, v.y * s);
}

static __inline__ GPoint2D
GPoint2DAdd(const GPoint2D& v1, const GPoint2D& v2)
{
    return GPoint2D(v1.x + v2.x, v1.y + v2.y);
}

static __inline__ GPoint2D
GPoint2DSub(const GPoint2D& v1, const GPoint2D& v2)
{
    return GPoint2D(v1.x - v2.x, v1.y - v2.y);
}

static __inline__ float
GPoint2DDot(const GPoint2D& v1, const GPoint2D& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

static __inline__ float
GPoint2DLengthSQ(const GPoint2D& v)
{
    return GPoint2DDot(v, v);
}

static __inline__ float
GPoint2DLength(const GPoint2D& v)
{
    return sqrtf(GPoint2DLengthSQ(v));
}

static __inline__ float
GPoint2DDistance(const GPoint2D& p1, const GPoint2D& p2)
{
    return GPoint2DLength(GPoint2DSub(p1, p2));
}

static __inline__ float
GPoint2DDistanceSQ(const GPoint2D& p1, const GPoint2D& p2)
{
    return GPoint2DLengthSQ(GPoint2DSub(p1, p2));
}

struct __GSize2D {
    float width;
    float height;
    
    __GSize2D() :width(0), height(0) {}
    __GSize2D(float _w, float _h) :width(_w), height(_h) {}
    
    float getW() const {return width;}
    float getH() const {return height;}
    
    void setW(float value) {width = value;}
    void setH(float value) {height = value;}
};

typedef struct __GSize2D GSize2D;



struct __GCircle2D {
    GPoint2D center;
    float radius;
    
    __GCircle2D() :center(GPoint2D()), radius(0) {}
    __GCircle2D(GPoint2D _c, float _r) :center(_c), radius(_r) {}
    
    GPoint2D getCenter() const {return center;}
    float getRadius() const {return radius;}
    
    void setCenter(GPoint2D value) {center = value;}
    void setRadius(float value) {radius = value;}
};

typedef struct __GCircle2D GCircle2D;



struct __GRect2D {
    GPoint2D   origin;
    GSize2D    size;
    
    __GRect2D() :origin(GPoint2D()), size(GSize2D()) {}
    __GRect2D(float x, float y, float width, float height) :origin(GPoint2D(x, y)), size(GSize2D(width, height)) {}
    __GRect2D(const __BoundingBox &boundingBox);
    
    float getX() const {return origin.x;}
    float getY() const {return origin.y;}
    
    float getW() const {return size.width;}
    float getH() const {return size.height;}
    
    void setX(float value) {origin.x = value;}
    void setY(float value) {origin.y = value;}
    
    void setW(float value) {size.width = value;}
    void setH(float value) {size.height = value;}
    
    GPoint2D GetCenter() const { return GPoint2D(origin.x + size.width/2, origin.y + size.height/2);}
    
    bool ContainsPoint(const GPoint2D &point) const;

};

typedef struct __GRect2D GRect2D;



struct __BoundingBox {
    GPoint2D min;
    GPoint2D max;
    
    __BoundingBox()
    {}
    
    __BoundingBox(const GRect2D &rect)
    :min(rect.origin)
    ,max(GPoint2D(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height))
    {}
    
    GPoint2D GetCenter() const {return GPoint2D((min.x + max.x)/2.0f, (min.y + max.y)/2.0f); }
    GPoint2D GetSize() const {return GPoint2D(- (min.x - max.x), -(min.y - max.y)); }
    
    void AddBoundingBox(__BoundingBox bb)
    {
        min.x = ____min(min.x, bb.min.x);
        min.y = ____min(min.y, bb.min.y);
        max.x = ____max(max.x, bb.max.x);
        max.y = ____max(max.y, bb.max.y);
    }
    
};
typedef struct __BoundingBox BoundingBox;

static __inline__ BoundingBox BoundingBoxMake(float x1, float y1, float x2, float y2) {
    BoundingBox rect;
    rect.min.x = x1;
    rect.min.y = y1;
    rect.max.x = x2;
    rect.max.y = y2;
    return rect;
};

static __inline__ BoundingBox BoundingBoxMake(GPoint2D min, GPoint2D max) {
    BoundingBox rect;
    rect.min = min;
    rect.max = max;
    return rect;
};

static __inline__ BoundingBox BoundingBoxMake(GRect2D rect) {
    BoundingBox result;
    result.min = GPoint2D(rect.origin.x, rect.origin.y);
    result.max = GPoint2D(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
    return result;
};

GRect2D RectWithCenterAndRadius(GPoint2D center, float radius);

class GUtils {
public:
    static std::vector<GPoint2D> CalculateLine(const GPoint2D &from, const GPoint2D &to);
    static GRect2D ClampRectToOuterRect(const GRect2D &rect, const GRect2D &outerRect);
};


static __inline__ bool GRect2DContainsPointInt(const GRect2D& rect, const GPoint2D& point)
{
    return ((int)point.x >= (int)rect.origin.x &&
        (int)point.y >= (int)rect.origin.y &&
        (int)point.x < (int)(rect.origin.x + rect.size.width) &&
        (int)point.y < (int)(rect.origin.y + rect.size.height));
}

class AIUtils {

public:

    static GRect2D ExpandRect(const GRect2D& rect, float expand);
    static std::vector<GPoint2D> GetPossiblePointsAroundPoint(const GPoint2D& point, const GRect2D& inRect, std::function<bool(const GPoint2D&)> checFunction, int maximumNumber, int checkCount);
    static GRect2D ClampRectToArea(const GRect2D& rect, const GRect2D& area);
    static GRect2D RectFromBoundingBox(const BoundingBox& bb);
    static float GetIntersectionArea(const BoundingBox& f, const BoundingBox& s);
    static float GetIntersectionArea(const GRect2D& first, const GRect2D& second);
};

#endif
