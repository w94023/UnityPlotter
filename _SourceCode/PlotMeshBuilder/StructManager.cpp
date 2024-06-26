#include "pch.h"
#include "StructManager.h"
#include <cmath>

float Max(float a, float b)
{
    return (a > b) ? a : b;
}

float Min(float a, float b)
{
    return (a < b) ? a : b;
}

float Lerp(float a, float b, float t)
{
    // t가 inf일 경우 min, max 값으로 b-a의 10배에 해당하는 큰 값을 반환
    if (std::isinf(t)) return Max(a - (b - a) * 10.0f, Min(b + (b - a) * 10.0f, a + (b - a) * t));
    else               return a + (b - a) * t;
}

float InverseLerp(float a, float b, float value)
{
    // a와 b가 같은 경우, 정의할 수 없으므로 0 또는 1을 반환
    if (a == b) return 0.0f;

    return (value - a) / (b - a);
}

// == 연산자 오버로딩
bool Vector2::operator==(const Vector2& other) const
{
    const float epsilon = 0.00001f;
    return (std::abs(x - other.x) < epsilon &&
            std::abs(y - other.y) < epsilon);
}

// != 연산자 오버로딩
bool Vector2::operator!=(const Vector2& other) const {
    return !(*this == other);
}

// [] 인덱싱 오버로딩
float& Vector2::operator[](int index) {
    switch (index) {
        case 0: return x;
        case 1: return y;
        default: throw std::out_of_range("Index out of range");
    }
}

void RangeInfo::GiveOffset(float offset)
{
    xLimit[0] -= offset; xLimit[1] += offset;
    yLimit[0] -= offset; yLimit[1] += offset;
    zLimit[0] -= offset; zLimit[1] += offset;
}

void Vector3::Print()
{
    std::cout << x << ", " << y << ", " << z << std::endl;
}

float Vector3::Length() const
{
    // 벡터 길이 계산
    return std::sqrt(x * x + y * y + z * z);
}

void Vector3::Normalize()
{
    // Normalize 메서드
    float length = Length();
    if (length != 0) {
        x /= length;
        y /= length;
        z /= length;
    }
}

Vector3 Vector3::RemoveZValue()
{
    Vector3 newPoint;
	newPoint.x = x;
	newPoint.y = y;
	return newPoint;
}

void Vector3::Clamp(RangeInfo rangeInfo)
{
    if (std::isinf(x)) {
        if      (x > 0) x = rangeInfo.xLimit[1];
        else if (x < 0) x = rangeInfo.xLimit[0];
    }
    if (std::isinf(y)) {
        if      (y > 0) y = rangeInfo.yLimit[1];
        else if (y < 0) y = rangeInfo.yLimit[0];
    }
    if (std::isinf(z)) {
        if      (z > 0) z = rangeInfo.zLimit[1];
        else if (z < 0) z = rangeInfo.zLimit[0];
    }
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2)
{
    // Cross Product 정적 메서드
    return Vector3(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
    // Dot Product 정적 메서드
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// == 연산자 오버로딩
bool Vector3::operator==(const Vector3& other) const
{
    const float epsilon = 0.00001f;
    return (std::abs(x - other.x) < epsilon &&
            std::abs(y - other.y) < epsilon &&
            std::abs(z - other.z) < epsilon
        );
}

// != 연산자 오버로딩
bool Vector3::operator!=(const Vector3& other) const {
    return !(*this == other);
}

// [] 인덱싱 오버로딩
float& Vector3::operator[](int index) {
    switch (index) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range("Index out of range");
    }
}

Quaternion Quaternion::Inverse() const
{
    // Inverse 메서드
    float normSquared = x * x + y * y + z * z + w * w;
    return Quaternion(-x / normSquared, -y / normSquared, -z / normSquared, w / normSquared);
}

Quaternion Quaternion::Euler(float y, float x, float z)
{
    // Euler to Quaternion 정적 메서드

    // 각도를 라디안으로 변환
    x = x * (float)M_PI / 180.0f;
    y = y * (float)M_PI / 180.0f;
    z = z * (float)M_PI / 180.0f;

    float cx = cos(x * 0.5f);
    float sx = sin(x * 0.5f);
    float cy = cos(y * 0.5f);
    float sy = sin(y * 0.5f);
    float cz = cos(z * 0.5f);
    float sz = sin(z * 0.5f);

    Quaternion q;
    q.w = cx * cy * cz + sx * sy * sz;
    q.x = cx * sy * cz + sx * cy * sz;
    q.y = sx * cy * cz - cx * sy * sz;
    q.z = cx * cy * sz - sx * sy * cz;

    return q;
}

Color Lerp(Color c1, Color c2, float t)
{
    Color newColor;
    newColor.r = Lerp(c1.r, c2.r, t);
    newColor.g = Lerp(c1.g, c2.g, t);
    newColor.b = Lerp(c1.b, c2.b, t);
    newColor.a = Lerp(c1.a, c2.a, t);
    return newColor;
}

// == 연산자 오버로딩
bool Color::operator==(const Color& other) const
{
    const float epsilon = 0.00001f;
    return (std::abs(r - other.r) < epsilon &&
            std::abs(g - other.g) < epsilon &&
            std::abs(b - other.b) < epsilon &&
            std::abs(a - other.a) < epsilon);
}

// != 연산자 오버로딩
bool Color::operator!=(const Color& other) const {
    return !(*this == other);
}

void Gradient::SetColorKeys(GradientColorKey* colorKeysVal, int colorKeyCountVal)
{
    colorKeys.clear();
    for (int i = 0; i < colorKeyCountVal; i++) {
        colorKeys.push_back(colorKeysVal[i]);
    }
}

void Gradient::SetAlphaKeys(GradientAlphaKey* alphaKeysVal, int alphaKeyCountVal)
{
    alphaKeys.clear();
    for (int i = 0; i < alphaKeyCountVal; i++) {
        alphaKeys.push_back(alphaKeysVal[i]);
    }
}
    
Color Gradient::Evaluate(float time)
{
    if (time < 0.0f) time = 0.0f;
	if (time > 1.0f) time = 1.0f;

	if (colorKeys.size() == 0) return Color(1.0f, 1.0f, 1.0f, 1.0f);

    int startIndex = -1;
    for (int i = colorKeys.size() - 1; i >= 0; i--) {
        if (time >= colorKeys[i].time) {
            startIndex = i;
            break;
        }
    }

    if (startIndex == -1) return colorKeys[0].color;

    int endIndex = colorKeys.size();
    for (int i = 0; i < colorKeys.size(); i++) {
        if (time < colorKeys[i].time) {
            endIndex = i;
            break;
        }
    }

    if (endIndex == colorKeys.size()) return colorKeys[colorKeys.size() - 1].color;

    return Lerp(colorKeys[startIndex].color, colorKeys[endIndex].color, InverseLerp(colorKeys[startIndex].time, colorKeys[endIndex].time, time));
}

// 연산자 오버로딩
Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
    return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
    return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
    return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
Vector3 operator*(float scalar, const Vector3& v)
{
    return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
}
Vector3 operator/(const Vector3& v, float scalar)
{
    return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
}
Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
    float newX = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    float newY = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    float newZ = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    float newW = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    return Quaternion(newX, newY, newZ, newW);
}
Vector3 operator*(const Quaternion& q, const Vector3& v)
{
    // 쿼터니언의 순수 벡터 부분
    Vector3 u = { q.x, q.y, q.z };

    // v + 2.0 * cross(u, v) + 2.0 * q.w * cross(u, cross(u, v))
    Vector3 uv, uuv;
    uv.x = u.y * v.z - u.z * v.y;
    uv.y = u.z * v.x - u.x * v.z;
    uv.z = u.x * v.y - u.y * v.x;
    
    uuv.x = u.y * uv.z - u.z * uv.y;
    uuv.y = u.z * uv.x - u.x * uv.z;
    uuv.z = u.x * uv.y - u.y * uv.x;
    
    Vector3 result;
    result.x = v.x + 2.0f * (q.w * uv.x + uuv.x);
    result.y = v.y + 2.0f * (q.w * uv.y + uuv.y);
    result.z = v.z + 2.0f * (q.w * uv.z + uuv.z);

    return result;
}

Color operator+(const Color& c1, const Color& c2)
{
    return Color(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a);
}

Color operator/(const Color& c, float scalar)
{
    return Color(c.r / scalar, c.g / scalar, c.b / scalar, c.a / scalar);
}

bool PlotShapeConfig::IsShapeNotChanged(PlotShapeConfig config)
{
	// 멤버가 하나도 바뀌지 않았을 경우 true, 아닐 경우 false 반환
	return (autoScaling == config.autoScaling &&
			axisColor == config.axisColor &&
			axisRadius == config.axisRadius &&
			xTickSpan == config.xTickSpan &&
			yTickSpan == config.yTickSpan &&
			zTickSpan == config.zTickSpan &&
			tickLength == config.tickLength &&
			tickRadius == config.tickRadius &&
			origin == config.origin &&
			xLimit == config.xLimit &&
			yLimit == config.yLimit &&
			zLimit == config.zLimit &&
			rectSize == config.rectSize &&
			rotEuler == config.rotEuler &&
            isRender2D == config.isRender2D
        );
}