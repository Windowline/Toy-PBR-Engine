#ifndef MATRIX_DATA
#define MATRIX_DATA

#include "Vector.hpp"


#define MATRIX4_ARRAY_SIZE 16

constexpr double PI = 3.14159265358979323846264338327950288;


template <typename T>
struct Matrix3 {
    Matrix3()
    {
        x.x = 1; x.y = 0; x.z = 0;
        y.x = 0; y.y = 1; y.z = 0;
        z.x = 0; z.y = 0; z.z = 1;
    }
    Matrix3(const T* m)
    {
        x.x = m[0]; x.y = m[1]; x.z = m[2];
        y.x = m[3]; y.y = m[4]; y.z = m[5];
        z.x = m[6]; z.y = m[7]; z.z = m[8];
    }
    Matrix3 transposed() const
    {
        Matrix3 m;
        m.x.x = x.x; m.x.y = y.x; m.x.z = z.x;
        m.y.x = x.y; m.y.y = y.y; m.y.z = z.y;
        m.z.x = x.z; m.z.y = y.z; m.z.z = z.z;
        return m;
    }
    Vector3<T> operator * (const Vector3<T>& b) const
    {
        Vector3<T> v;
        v.x = x.x * b.x + x.y * b.y + x.z * b.z;
        v.y = y.x * b.x + y.y * b.y + y.z * b.z;
        v.z = z.x * b.x + z.y * b.y + z.z * b.z;
        return v;
    }
    const T* Pointer() const
    {
        return &x.x;
    }
    Vector3<T> x;
    Vector3<T> y;
    Vector3<T> z;
};

template <typename T>
struct Matrix4 {
    Matrix4()
    {
        x.x = 1; x.y = 0; x.z = 0; x.w = 0;
        y.x = 0; y.y = 1; y.z = 0; y.w = 0;
        z.x = 0; z.y = 0; z.z = 1; z.w = 0;
        w.x = 0; w.y = 0; w.z = 0; w.w = 1;
    }
    Matrix4(const Matrix3<T>& m)
    {
        x.x = m.x.x; x.y = m.x.y; x.z = m.x.z; x.w = 0;
        y.x = m.y.x; y.y = m.y.y; y.z = m.y.z; y.w = 0;
        z.x = m.z.x; z.y = m.z.y; z.z = m.z.z; z.w = 0;
        w.x = 0; w.y = 0; w.z = 0; w.w = 1;
    }
    Matrix4(const T* m)
    {
        x.x = m[0];  x.y = m[1];  x.z = m[2];  x.w = m[3];
        y.x = m[4];  y.y = m[5];  y.z = m[6];  y.w = m[7];
        z.x = m[8];  z.y = m[9];  z.z = m[10]; z.w = m[11];
        w.x = m[12]; w.y = m[13]; w.z = m[14]; w.w = m[15];
    }
    Matrix4 operator * (const Matrix4& b) const
    {
        Matrix4 m;
        m.x.x = x.x * b.x.x + x.y * b.y.x + x.z * b.z.x + x.w * b.w.x;
        m.x.y = x.x * b.x.y + x.y * b.y.y + x.z * b.z.y + x.w * b.w.y;
        m.x.z = x.x * b.x.z + x.y * b.y.z + x.z * b.z.z + x.w * b.w.z;
        m.x.w = x.x * b.x.w + x.y * b.y.w + x.z * b.z.w + x.w * b.w.w;
        m.y.x = y.x * b.x.x + y.y * b.y.x + y.z * b.z.x + y.w * b.w.x;
        m.y.y = y.x * b.x.y + y.y * b.y.y + y.z * b.z.y + y.w * b.w.y;
        m.y.z = y.x * b.x.z + y.y * b.y.z + y.z * b.z.z + y.w * b.w.z;
        m.y.w = y.x * b.x.w + y.y * b.y.w + y.z * b.z.w + y.w * b.w.w;
        m.z.x = z.x * b.x.x + z.y * b.y.x + z.z * b.z.x + z.w * b.w.x;
        m.z.y = z.x * b.x.y + z.y * b.y.y + z.z * b.z.y + z.w * b.w.y;
        m.z.z = z.x * b.x.z + z.y * b.y.z + z.z * b.z.z + z.w * b.w.z;
        m.z.w = z.x * b.x.w + z.y * b.y.w + z.z * b.z.w + z.w * b.w.w;
        m.w.x = w.x * b.x.x + w.y * b.y.x + w.z * b.z.x + w.w * b.w.x;
        m.w.y = w.x * b.x.y + w.y * b.y.y + w.z * b.z.y + w.w * b.w.y;
        m.w.z = w.x * b.x.z + w.y * b.y.z + w.z * b.z.z + w.w * b.w.z;
        m.w.w = w.x * b.x.w + w.y * b.y.w + w.z * b.z.w + w.w * b.w.w;
        return m;
    }
    Matrix4 operator * (const T b) const
    {
        Matrix4 m;
        m.x.x = x.x * b;
        m.x.y = x.y * b;
        m.x.z = x.z * b;
        m.x.w = x.w * b;
        m.y.x = y.x * b;
        m.y.y = y.y * b;
        m.y.z = y.z * b;
        m.y.w = y.w * b;
        m.z.x = z.x * b;
        m.z.y = z.y * b;
        m.z.z = z.z * b;
        m.z.w = z.w * b;
        m.w.x = w.x * b;
        m.w.y = w.y * b;
        m.w.z = w.z * b;
        m.w.w = w.w * b;
        return m;
    }

    Vector4<T> operator * (const Vector4<T>& b) const
    {
        Vector4<T> v;
        v.x = x.x * b.x + x.y * b.y + x.z * b.z + x.w * b.w;
        v.y = y.x * b.x + y.y * b.y + y.z * b.z + y.w * b.w;
        v.z = z.x * b.x + z.y * b.y + z.z * b.z + z.w * b.w;
        v.w = w.x * b.x + w.y * b.y + w.z * b.z + w.w * b.w;
        return v;
    }

    Matrix4& operator *= (const Matrix4& b)
    {
        Matrix4 m = *this * b;
        return (*this = m);
    }

    Vector4<T> multiplication1n4(const Vector4<T>& b) const
    {
        Vector4<T> v;
        v.x = x.x * b.x + y.x * b.y + z.x * b.z + w.x * b.w;
        v.y = x.y * b.x + y.y * b.y + z.y * b.z + w.y * b.w;
        v.z = x.z * b.x + y.z * b.y + z.z * b.z + w.z * b.w;
        v.w = x.w * b.x + y.w * b.y + z.w * b.z + w.w * b.w;
        return v;
    }

    Matrix4 transposed() const
    {
        Matrix4 m;
        m.x.x = x.x; m.x.y = y.x; m.x.z = z.x; m.x.w = w.x;
        m.y.x = x.y; m.y.y = y.y; m.y.z = z.y; m.y.w = w.y;
        m.z.x = x.z; m.z.y = y.z; m.z.z = z.z; m.z.w = w.z;
        m.w.x = x.w; m.w.y = y.w; m.w.z = z.w; m.w.w = w.w;
        return m;
    }

    Vector3<T> getScale() const {
        return Vector3<T>(x.x, y.y, z.z);
    }

    Vector3<T> getTranslation() const {
        return Vector3<T>(w.x, w.y, w.z);
    }


    Matrix4 invert() const {
        T d;
        return invert(d);
    };


    Matrix4 invert(T& det) const
    {
        Matrix4 inv;
        const T* m = &x.x;

        //inv.x.x = y.y*z.z*w.w - y.y*z.w*w.z - z.y*y.z*w.w + z.y*y.w*w.z + w.y*y.z*z.w - w.y*y.w*z.z;
        inv.x.x =  m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15] + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
        inv.x.y = -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15] - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
        inv.x.z =  m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15] + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
        inv.x.w = -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11] - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];

        inv.y.x = -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15] - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
        inv.y.y =  m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15] + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
        inv.y.z = -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15] - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
        inv.y.w =  m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11] + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];

        inv.z.x =  m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15] + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
        inv.z.y = -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15] - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
        inv.z.z =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15] + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
        inv.z.w = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11] - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];

        inv.w.x = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14] - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
        inv.w.y = m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14] + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
        inv.w.z = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14] - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
        inv.w.w = m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10] + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

        det = m[0]*inv.x.x + m[1]*inv.y.x + m[2]*inv.z.x +m[3]*inv.w.x;
        if (det == 0) {
            return Matrix4();
        }

        det = 1.0f/det;

        inv = inv * det;

        return inv;
    }


    const T* pointer() const
    {
        return &x.x;
    }

    void identity()
    {
        x.x = 1; x.y = 0; x.z = 0; x.w = 0;
        y.x = 0; y.y = 1; y.z = 0; y.w = 0;
        z.x = 0; z.y = 0; z.z = 1; z.w = 0;
        w.x = 0; w.y = 0; w.z = 0; w.w = 1;
    }

    void translate(T x_, T y_, T z_)
    {
        w.x = x.x * x_ + y.x * y_ + z.x * z_ + w.x;
        w.y = x.y * x_ + y.y * y_ + z.y * z_ + w.y;
        w.z = x.z * x_ + y.z * y_ + z.z * z_ + w.z;
        w.w = x.w * x_ + y.w * y_ + z.w * z_ + w.w;
    }

    void scale(T x_, T y_, T z_)
    {
        x.x = x.x * x_;
        x.y = x.y * x_;
        x.z = x.z * x_;
        x.w = x.w * x_;
        y.x = y.x * y_;
        y.y = y.y * y_;
        y.z = y.z * y_;
        y.w = y.w * y_;
        z.x = z.x * z_;
        z.y = z.y * z_;
        z.z = z.z * z_;
        z.w = z.w * z_;
    }


    static Matrix4<T> Translate(T x, T y, T z)
    {
        // row major matrix.
        Matrix4 m;
        m.x.x = 1; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x = 0; m.y.y = 1; m.y.z = 0; m.y.w = 0;
        m.z.x = 0; m.z.y = 0; m.z.z = 1; m.z.w = 0;
        m.w.x = x; m.w.y = y; m.w.z = z; m.w.w = 1;
        return m;
    }

    static Matrix4<T> Scale(T s)
    {
        Matrix4 m;
        m.x.x = s; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x = 0; m.y.y = s; m.y.z = 0; m.y.w = 0;
        m.z.x = 0; m.z.y = 0; m.z.z = s; m.z.w = 0;
        m.w.x = 0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }

    static Matrix4<T> Scale(T x, T y, T z)
    {
        Matrix4 m;
        m.x.x = x; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x = 0; m.y.y = y; m.y.z = 0; m.y.w = 0;
        m.z.x = 0; m.z.y = 0; m.z.z = z; m.z.w = 0;
        m.w.x = 0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }

    static Matrix4<T> RotateZ(T degrees)
    {
        T radians = degrees * M_PI / 180.0f;
        T s = std::sin(radians);
        T c = std::cos(radians);

        Matrix4 m;
        m.x.x =  c; m.x.y = -s; m.x.z = 0; m.x.w = 0;
        m.y.x =  s; m.y.y = c; m.y.z = 0; m.y.w = 0;
        m.z.x =  0; m.z.y = 0; m.z.z = 1; m.z.w = 0;
        m.w.x =  0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }

    static Matrix4<T> RotateY(T degrees)
    {
        T radians = degrees * M_PI / 180.0f;
        T s = std::sin(radians);
        T c = std::cos(radians);

        Matrix4 m;
        m.x.x =  c; m.x.y = 0; m.x.z = s; m.x.w = 0;
        m.y.x =  0; m.y.y = 1; m.y.z = 0; m.y.w = 0;
        m.z.x = -s; m.z.y = 0; m.z.z = c; m.z.w = 0;
        m.w.x =  0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }

    static Matrix4<T> Rotate(T degrees, T x, T y, T z)
    {
        T radians = degrees * M_PI / 180.0f;
        T s = std::sin(radians);
        T c = std::cos(radians);

        Matrix4 m;
        m.x.x =  x*x*(1.0f-c) + c; m.x.y = y*x*(1.0f-c) + z*s; m.x.z = x*z*(1.0f-c) - y*s; m.x.w = 0;
        m.y.x =  x*y*(1.0f-c) - z*s; m.y.y = y*y*(1.0f-c) + c; m.y.z = y*z*(1.0f-c) + x*s; m.y.w = 0;
        m.z.x =  x*z*(1.0f-c) + y*s; m.z.y = y*z*(1.0f-c) - x*s; m.z.z = z*z*(1.0f-c) + c; m.z.w = 0;
        m.w.x =  0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }

    static Matrix4<T> AxisRotate(const Vector3<T>& axis, T degrees)
    {

        T radians = degrees * M_PI / 180.0f;
        T x = axis.x;
        T y = axis.y;
        T z = axis.z;

        T n = x*x + y*y + z*z;

        if (n != 1.0) {

            n = sqrt(n);

            if (n > 0.000001) {
                n = 1.0 / n;
                x *= n;
                y *= n;
                z *= n;
            }
        }

        T c = std::cos(radians);
        T s = std::sin(radians);

        T t = 1.0 - c;
        T tx = t * x;
        T ty = t * y;
        T tz = t * z;
        T txy = tx * y;
        T txz = tx * z;
        T tyz = ty * z;
        T sx = s * x;
        T sy = s * y;
        T sz = s * z;

        T m[16];

        m[0] = c + tx*x;
        m[1] = txy + sz;
        m[2] = txz - sy;
        m[3] = 0.0;

        m[4] = txy - sz;
        m[5] = c + ty*y;
        m[6] = tyz + sx;
        m[7] = 0.0;

        m[8] = txz + sy;
        m[9] = tyz - sx;
        m[10] = c + tz*z;
        m[11] = 0.0;

        m[12] = 0.0;
        m[13] = 0.0;
        m[14] = 0.0;
        m[15] = 1.0;

        m[0] = c + tx*x;
        m[1] = txy + sz;
        m[2] = txz - sy;
        m[3] = 0.0;

        m[4] = txy - sz;
        m[5] = c + ty*y;
        m[6] = tyz + sx;
        m[7] = 0.0;

        m[8] = txz + sy;
        m[9] = tyz - sx;
        m[10] = c + tz*z;
        m[11] = 0.0;

        m[12] = 0.0;
        m[13] = 0.0;
        m[14] = 0.0;
        m[15] = 1.0;

        Matrix4<T> ret(m);
        return ret.transposed();
    }


    static Matrix4<T> RotateX(T degrees)
    {
        T radians = degrees * M_PI / 180.0f;
        T s = std::sin(radians);
        T c = std::cos(radians);

        Matrix4 m;
        m.x.x =  1; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x =  0; m.y.y = c; m.y.z =-s; m.y.w = 0;
        m.z.x =  0; m.z.y = s; m.z.z = c; m.z.w = 0;
        m.w.x =  0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }

    static Matrix4<T> Frustum(T screenW, T screenH, T fovy, T near, T far) {
        T aspect = screenW / screenH;
        T frustumH = tanf(fovy / 360.0 * M_PI) * near;
        T frustumW = frustumH * aspect;
        return Matrix4::Frustum(-frustumW, frustumW, -frustumH, frustumH, near, far);
    }


    static Matrix4<T> Frustum(T tLeft, T tRight, T tBottom, T tTop, T tNear, T tFar) {
        T a = 2 * tNear / (tRight - tLeft);
        T b = 2 * tNear / (tTop - tBottom);
        T c = (tRight + tLeft) / (tRight - tLeft);
        T d = (tTop + tBottom) / (tTop - tBottom);
        T e = - (tFar + tNear) / (tFar - tNear);
        T f = -2 * tFar * tNear / (tFar - tNear);
        Matrix4 m;
        m.x.x = a; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x = 0; m.y.y = b; m.y.z = 0; m.y.w = 0;
        m.z.x = c; m.z.y = d; m.z.z = e; m.z.w = -1;
        m.w.x = 0; m.w.y = 0; m.w.z = f; m.w.w = 0;
        return m;
    }

    static Matrix4<T> Ortho(T xMin, T xMax, T yMin, T yMax, T zMin, T zMax) {
        Matrix4 m;

        T xDiff = xMax - xMin;
        T yDiff = yMax - yMin;
        T zDiff = zMax - zMin;

        m.x.x = 2.0f / xDiff;
        m.y.y = 2.0f / yDiff;
        m.z.z = -(2.0f / zDiff);               // BUG: sign inverted.

        m.w.x = -(xMax + xMin) / xDiff;
        m.w.y = -(yMax + yMin) / yDiff;
        m.w.z = -(zMax + zMin) / zDiff;
        m.w.w = 1.0f;

        return m;
    }

    T* data() {
        return &x.x;
    }

    Vector4<T> x;
    Vector4<T> y;
    Vector4<T> z;
    Vector4<T> w;
};


typedef Matrix3<float> mat3;
typedef Matrix4<float> mat4;


#endif
