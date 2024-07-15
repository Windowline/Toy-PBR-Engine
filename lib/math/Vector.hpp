#ifndef Vector_hpp
#define Vector_hpp

#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

template<typename T>
struct Vector2 {

    constexpr Vector2() : x(0), y(0) {}

    constexpr Vector2(T x, T y) : x(x), y(y) {}

    constexpr Vector2(Vector2<T> const& v) : x(v.x), y(v.y) {}

    T Dot(const Vector2& v) const {
        return x * v.x + y * v.y;
    }

    Vector2 operator+(const Vector2& v) const {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2<T> operator+(float s) const {
        return Vector2(x + s, y + s);
    }

    Vector2 operator-(const Vector2& v) const {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2<T> operator-(float s) const {
        return Vector2(x - s, y - s);
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }

    Vector2<T>& operator+=(const Vector2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    void operator-=(const Vector2& v) {
        *this = Vector2(x - v.x, y - v.y);
    }


    Vector2<T> operator*(float s) const {
        return Vector2(x * s, y * s);
    }

    Vector2<T>& operator*=(float s) {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2<T> operator*(const Vector2& v) const {
        return Vector2(x * v.x, y * v.y);
    }

    Vector2<T>& operator*=(const Vector2& v) {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    Vector2 operator/(T s) const {
        return Vector2(x / s, y / s);
    }

    Vector2<T>& operator/=(float s) {
        x = x / s;
        y = y / s;
        return *this;
    }

    Vector2<T> operator/(const Vector2& v) const {
        return Vector2(x / v.x, y / v.y);
    }

    Vector2<T>& operator/=(const Vector2& v) {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    Vector2<T>& ceil() {
        x = std::ceil(x);
        y = std::ceil(y);
        return *this;
    }

    Vector2<T>& floor() {
        x = std::floor(x);
        y = std::floor(y);
        return *this;
    }

    Vector2<T>& round() {
        x = std::round(x);
        y = std::round(y);
        return *this;
    }

    Vector2<T>& operator=(const Vector2<T>& v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    void normalize() {
        float s = 1.0f / length();
        x *= s;
        y *= s;
    }

    Vector2 normalized() const {
        Vector2 v = *this;
        v.Normalize();
        return v;
    }

    Vector2 perpendicular() const {
        // ccw
        Vector2 v(-y, x);
        return v;
    }

    T distance(Vector2<T> const& p) const {
        return std::sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
    }

    T distanceSquared(Vector2<T> const& p) const {
        return ((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
    }

    T lengthSquared() const {
        return x * x + y * y;
    }

    T length() const {
        return sqrt(lengthSquared());
    }

    operator Vector2<float>() const {
        return Vector2<float>(x, y);
    }

    bool operator<(const Vector2& v) const {
        return x < v.x && y < v.y;
    }

    bool operator==(const Vector2& v) const {
        return x == v.x && y == v.y;
    }

    bool operator!=(const Vector2& v) const {
        return x != v.x || y != v.y;
    }

    Vector2 lerp(float t, const Vector2& v) const {
        return Vector2(x * (1 - t) + v.x * t,
                       y * (1 - t) + v.y * t);
    }

    const T* pointer() const {
        return &x;
    }

    T x;
    T y;
};

template<typename T>
struct Vector3 {
    Vector3() : x(0), y(0), z(0) {}

    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

    Vector3(const Vector2<T>& v, T z) : x(v.x), y(v.y), z(z) {}

    Vector3(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z) {}

    T& operator[](int i) {
        if (i == 0)
            return x;
        else if (i == 1)
            return y;
        else
            return z;
    }

    T lengthSquared() const {
        return x * x + y * y + z * z;
    }

    T length() const {
        return std::sqrt(lengthSquared());
    }

    T distance(Vector3<T> const& p) const {
        return std::sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
    }

    T distanceSquared(Vector3<T> const& p) const {
        return ((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
    }

    void normalize() {
        float s = 1.0f / length();
        x *= s;
        y *= s;
        z *= s;
    }

    Vector3 normalized() const {
        Vector3 v = *this;
        v.normalize();
        return v;
    }

    Vector3 cross(const Vector3& v) const {
        return Vector3(y * v.z - z * v.y,
                       z * v.x - x * v.z,
                       x * v.y - y * v.x);
    }

    T dot(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator*(const Vector3& v) const {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    void operator+=(const Vector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void operator-=(const Vector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    void operator/=(T s) {
        x /= s;
        y /= s;
        z /= s;
    }

    void operator*=(T s) {
        x *= s;
        y *= s;
        z *= s;
    }

    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    Vector3 operator*(T const& s) const {
        return Vector3(x * s, y * s, z * s);
    }

    Vector3& operator*=(T const& s) const {
        x = x * s;
        y = y * s;
        z = z * s;
        return *this;
    }

    Vector3 operator/(T const& s) const {
        return Vector3(x / s, y / s, z / s);
    }

    bool operator==(const Vector3& v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(const Vector3& v) const {
        return x != v.x || y != v.y || z != v.z;
    }

    Vector3 lerp(float t, const Vector3& v) const {
        return Vector3(x * (1 - t) + v.x * t,
                       y * (1 - t) + v.y * t,
                       z * (1 - t) + v.z * t);
    }

    const T* pointer() const {
        return &x;
    }

    T x;
    T y;
    T z;
};

template<typename T>
struct Vector4 {
    Vector4() : x(0), y(0), z(0), w(1) {}

    Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    Vector4(const Vector2<T>& v, T z, T w) : x(v.x), y(v.y), z(z), w(w) {}

    Vector4(const Vector3<T>& v, T w) : x(v.x), y(v.y), z(v.z), w(w) {}

    Vector4(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    T dot(const Vector4& v) const {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }

    Vector4 operator-() const {
        return Vector4(-x, -y, -z, -w);
    }

    Vector4 lerp(float t, const Vector4& v) const {
        return Vector4(x * (1 - t) + v.x * t,
                       y * (1 - t) + v.y * t,
                       z * (1 - t) + v.z * t,
                       w * (1 - t) + v.w * t);
    }

    Vector4 operator+(const Vector4& v) const {
        return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
    }

    Vector4 operator-(const Vector4& v) const {
        return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
    }

    bool operator==(const Vector4& v) const {
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    bool operator!=(const Vector4& v) const {
        return x != v.x || y != v.y || z != v.z || w != v.w;
    }

    Vector4 operator*(T s) const {
        return Vector4(x * s, y * s, z * s, w * s);
    }

    Vector3<T> xyz() const {
        return Vector3<T>(x, y, z);
    }

    Vector2<T> xy() const {
        return Vector2<T>(x, y);
    }

    const T* pointer() const {
        return &x;
    }

    T x;
    T y;
    T z;
    T w;
};



typedef Vector2<int> ivec2;
typedef Vector3<int> ivec3;
typedef Vector4<int> ivec4;

typedef Vector2<float> vec2;
typedef Vector3<float> vec3;
typedef Vector4<float> vec4;



//#ifndef Ray_hpp
//#define Ray_hpp
//
//#include "Vector.hpp"

/**
 * 시작점, 방향을 가진 광선 정보입니다.
 */
class Ray {
public:
    Ray(vec3 origin, vec3 direction) : _origin(std::move(origin)), _direction(std::move(direction)) {}

    vec3 origin() const {
        return _origin;
    }

    vec3 direction() const {
        return _direction;
    }

    bool intersectWithSphere(vec3 sphereOrigin, float sphereRadius, vec3& outPos) const {
        vec3 rayToSphereOrigin = sphereOrigin - _origin;
        vec3 rayDir = _direction;
        float tca = rayToSphereOrigin.dot(rayDir);

        if (tca < 0.0) {
            return false;
        }

        float dSquare = rayToSphereOrigin.dot(rayToSphereOrigin) - (tca * tca);
        float rSquare = sphereRadius * sphereRadius;

        if (dSquare > rSquare) {
            return false;
        }

        float thc = sqrt(rSquare - dSquare);
        float t = tca - thc;
        vec3 normalized = _direction.normalized();
        outPos = _origin + vec3(normalized.x * t, normalized.y * t, normalized.z * t);
        return true;
    }

    //TODO
    bool intersectWithBox(vec3 boxMinPos, vec3 boxMaxPos, vec3& outPos) {
        return false;
    }

    //TODO
    bool intersectWithPlane(vec3 planeP1, vec3 planeP2, vec3 planeP3, vec3 planeP4, vec3& outPos) {
        return false;
    }

private:
    void normalize() {
        float normalizeFactor =
                1.0 / sqrt(_direction.x * _direction.x + _direction.y * _direction.y + _direction.z * _direction.z);
        if (normalizeFactor != 1.0) {
            _direction.x *= normalizeFactor;
            _direction.y *= normalizeFactor;
            _direction.z *= normalizeFactor;
        }
    }

    vec3 _origin;
    vec3 _direction;
};

struct AABB {
    vec3 boundsMin = vec3(1e9, 1e9, 1e9);
    vec3 boundsMax = vec3(-1e9, -1e9, -1e9);

    void extend(const vec3& p) {
        boundsMin.x = min(p.x, boundsMin.x);
        boundsMin.y = min(p.y, boundsMin.y);
        boundsMin.z = min(p.z, boundsMin.z);

        boundsMax.x = max(p.x, boundsMax.x);
        boundsMax.y = max(p.y, boundsMax.y);
        boundsMax.z = max(p.z, boundsMax.z);
    }

    vec3 size() const {
        return vec3(boundsMax.x - boundsMin.x,
                    boundsMax.y - boundsMin.y,
                    boundsMax.z - boundsMin.z);
    }

    vec3 center() const {
        return (boundsMax + boundsMin) / 2.0;
    }



};

//#endif /* Ray_hpp */


#endif /* Vector_hpp */
