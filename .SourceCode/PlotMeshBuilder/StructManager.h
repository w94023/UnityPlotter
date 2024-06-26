#pragma once
#include "pch.h"
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES

#include <math.h>

typedef void (*EventCallback)();

float Max(float a, float b);

float Min(float a, float b);

float Lerp(float a, float b, float t);

float InverseLerp(float a, float b, float value);

struct Vector2
{
    float x, y;
    
    Vector2()                       : x(0.0f), y(0.0f) {}
    Vector2(float xVal, float yVal) : x(xVal), y(yVal) {}

    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;
    float& operator[](int index);
};

struct RangeInfo
{
    Vector2 xLimit;
    Vector2 yLimit;
    Vector2 zLimit;

    void GiveOffset(float offset);
};

struct Vector3
{
    float x, y, z;

    Vector3 ()                                   : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3 (float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}
    Vector3 (Vector2 v)                          : x(v.x),  y(v.y),  z(0.0f) {}

           void    Print();
           float   Length   () const;
           void    Normalize();
           Vector3 RemoveZValue();
           void    Clamp(RangeInfo rangeInfo);
    static Vector3 Cross    (const Vector3& v1, const Vector3& v2);
    static float   Dot      (const Vector3& v1, const Vector3& v2);

    bool operator==(const Vector3& other) const;
    bool operator!=(const Vector3& other) const;
    float& operator[](int index);
};

struct Quaternion
{
    float x, y, z, w;
    
    Quaternion()                                               : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float xVal, float yVal, float zVal, float wVal) : x(xVal), y(yVal), z(zVal), w(wVal) {}

           Quaternion Inverse() const;
    static Quaternion Euler  (float y, float x, float z);
};

struct Color
{
    float r, g, b, a;

    Color()                                               : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(float rVal, float gVal, float bVal, float aVal) : r(rVal), g(gVal), b(bVal), a(aVal) {}

    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;
};

Color Lerp(Color c1, Color c2, float t);

struct GradientAlphaKey
{
    float alpha;
    float time;

    GradientAlphaKey() : alpha(1.0f), time(1.0f) {}
};

struct GradientColorKey
{
    Color color;
    float time;

    GradientColorKey() : color(Color(1.0f, 1.0f, 1.0f, 1.0f)), time(0.0f) {}
};

struct Gradient
{
    std::vector<GradientColorKey> colorKeys;
    std::vector<GradientAlphaKey> alphaKeys;

    void  SetColorKeys(GradientColorKey* colorKeysVal, int colorKeyCountVal);
    void  SetAlphaKeys(GradientAlphaKey* alphaKeysVal, int alphaKeyCountVal);
    Color Evaluate    (float time);
};

enum DataType
{
    PlotData,
    MapData,
    AxisData,
    XAxisTickData,
    YAxisTickData,
    XAxisTextData,
    YAxisTextData
};

struct DataInfo
{
    int       linkCount;
    Color     color;
    float     radius;
    DataType  dataType;
    bool      isFillMesh; // fill mesh or plot mesh 구별
    float     zBasedScaler;
};

struct PlotShapeConfig
{
    int     autoScaling;
    Color   axisColor;
    float   axisRadius;

    float   xTickSpan;
    float   yTickSpan;
    float   zTickSpan;
    float   tickLength;
    float   tickRadius;

    Vector3 origin;
    Vector2 xLimit;
    Vector2 yLimit;
    Vector2 zLimit;
    Vector2 rectSize;
    Vector3 rotEuler;
    float   zProjectionScale;

    int isRender2D;

    bool IsShapeNotChanged(PlotShapeConfig config);
};

struct PlotDataConfig
{
    float radius;
    Color color;
    int   segments;
};

struct IndexedFloat
{
	float* value;
	int* originalIndex;
};

// 연산자 오버로딩
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float scalar, const Vector3& v);
Vector3 operator/(const Vector3& v, float scalar);
Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Vector3 operator*(const Quaternion& quat, const Vector3& v);
Color operator+(const Color& c1, const Color& c2);
Color operator/(const Color& c, float scalar);