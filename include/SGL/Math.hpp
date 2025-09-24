#pragma once

#include <cmath>
#include <algorithm>

namespace SmallGraphicsLayer::Math {

struct Vec2 {
    float x{0}, y{0};

    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}

    // basic arithmetic
    constexpr Vec2 operator+(const Vec2& r) const noexcept { return {x + r.x, y + r.y}; }
    constexpr Vec2 operator-(const Vec2& r) const noexcept { return {x - r.x, y - r.y}; }
    constexpr Vec2 operator-() const noexcept { return {-x, -y}; }

    constexpr bool operator==(const Vec2& r) const noexcept { return x == r.x && y == r.y; }
    constexpr Vec2& operator+=(const Vec2& r) noexcept { x += r.x; y += r.y; return *this; }
    constexpr Vec2& operator-=(const Vec2& r) noexcept { x -= r.x; y -= r.y; return *this; }

    // scalar ops
    constexpr Vec2 operator*(float s) const noexcept { return {x * s, y * s}; }
    constexpr Vec2 operator/(float s) const noexcept { return {x / s, y / s}; }
    constexpr Vec2& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
    constexpr Vec2& operator/=(float s) noexcept { x /= s; y /= s; return *this; }

    friend constexpr Vec2 operator*(float s, const Vec2& v) noexcept { return {v.x * s, v.y * s}; }

    // helpers
    constexpr float dot(const Vec2& r) const noexcept { return x * r.x + y * r.y; }
    float length() const noexcept { return std::sqrt(dot(*this)); }
    constexpr float length2() const noexcept { return dot(*this); }

    Vec2 normalized(float eps = 1e-6f) const noexcept {
        float len2 = length2();
        if (len2 <= eps * eps) return {0.f, 0.f};
        float inv = 1.f / std::sqrt(len2);
        return {x * inv, y * inv};
    }

    // 90 degree CCW perpendicular 
    constexpr Vec2 perp() const noexcept { return {-y, x}; }

    static constexpr Vec2 lerp(const Vec2& a, const Vec2& b, float t) noexcept {
        return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
    }
};

struct iVec2 {
    int x{0}, y{0};

    constexpr iVec2() = default;
    constexpr iVec2(int x_, int y_) : x(x_), y(y_) {}

    // equality
    constexpr bool operator==(const iVec2& r) const noexcept { return x == r.x && y == r.y; }

    // basic arithmetic
    constexpr iVec2 operator+(const iVec2& r) const noexcept { return {x + r.x, y + r.y}; }
    constexpr iVec2 operator-(const iVec2& r) const noexcept { return {x - r.x, y - r.y}; }
    constexpr iVec2 operator-() const noexcept { return {-x, -y}; }

    constexpr bool operator==(const iVec2& r) noexcept { return x == r.x && y == r.y; }
    constexpr iVec2& operator+=(const iVec2& r) noexcept { x += r.x; y += r.y; return *this; }
    constexpr iVec2& operator-=(const iVec2& r) noexcept { x -= r.x; y -= r.y; return *this; }

    // scalar ops (integer)
    constexpr iVec2 operator*(int s) const noexcept { return {x * s, y * s}; }
    constexpr iVec2& operator*=(int s) noexcept { x *= s; y *= s; return *this; }

    constexpr iVec2 operator/(int s) const noexcept { return {x / s, y / s}; }
    constexpr iVec2& operator/=(int s) noexcept { x /= s; y /= s; return *this; }
    friend constexpr iVec2 operator*(int s, const iVec2& v) noexcept { return {v.x * s, v.y * s}; }

    // helpers
    constexpr int dot(const iVec2& r) const noexcept { return x * r.x + y * r.y; }
    constexpr int length2() const noexcept { return dot(*this); }
    double length() const noexcept { return std::sqrt(static_cast<double>(length2())); }

    // 90 degree CCW perpendicular
    constexpr iVec2 perp() const noexcept { return {-y, x}; }

    // conversions to float vector for real-valued ops
    explicit constexpr operator Vec2() const noexcept { return Vec2{static_cast<float>(x), static_cast<float>(y)}; }

    // real-valued helpers that return Vec2
    Vec2 normalized(double eps = 1e-6) const noexcept {
        double len2 = static_cast<double>(length2());
        if (len2 <= eps * eps) return {0.f, 0.f};
        double inv = 1.0 / std::sqrt(len2);
        return {static_cast<float>(x * inv), static_cast<float>(y * inv)};
    }

    static Vec2 lerp(const iVec2& a, const iVec2& b, float t) noexcept {
        return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
    }
};

struct Vec3 {
    float x{0}, y{0}, z{0};

    constexpr Vec3() = default;
    constexpr Vec3(float x_, float y_, float z_ = 0) : x(x_), y(y_), z(z_) {}

    // basic arithmetic
    constexpr Vec3 operator+(const Vec3& r) const noexcept { return {x + r.x, y + r.y, z + r.z}; }
    constexpr Vec3 operator-(const Vec3& r) const noexcept { return {x - r.x, y - r.y, z - r.z}; }
    constexpr Vec3 operator-() const noexcept { return {-x, -y, -z}; }

    constexpr Vec3& operator+=(const Vec3& r) noexcept { x += r.x; y += r.y; z += r.z; return *this; }
    constexpr Vec3& operator-=(const Vec3& r) noexcept { x -= r.x; y -= r.y; z -= r.z; return *this; }

    // scalar ops
    constexpr Vec3 operator*(float s) const noexcept { return {x * s, y * s, z * s}; }
    constexpr Vec3 operator/(float s) const noexcept { return {x / s, y / s, z / s}; }
    constexpr Vec3& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
    constexpr Vec3& operator/=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }

    friend constexpr Vec3 operator*(float s, const Vec3& v) noexcept { return {v.x * s, v.y * s, v.z * s}; }

    // helpers
    constexpr float dot(const Vec3& r) const noexcept { return x * r.x + y * r.y + z * r.z; }
    constexpr Vec3 cross(const Vec3& r) const noexcept {
        return { y * r.z - z * r.y,
                 z * r.x - x * r.z,
                 x * r.y - y * r.x };
    }

    float length() const noexcept { return std::sqrt(dot(*this)); }
    constexpr float length2() const noexcept { return dot(*this); }

    Vec3 normalized(float eps = 1e-6f) const noexcept {
        float len2 = length2();
        if (len2 <= eps * eps) return {0.f, 0.f, 0.f};
        float inv = 1.f / std::sqrt(len2);
        return {x * inv, y * inv, z * inv};
    }

    static constexpr Vec3 lerp(const Vec3& a, const Vec3& b, float t) noexcept {
        return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t};
    }
};

struct Vec4 {
    float x{0}, y{0}, z{0}, w{0};

    constexpr Vec4() = default;
    constexpr Vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

    // basic arithmetic
    constexpr Vec4 operator+(const Vec4& r) const noexcept { return {x + r.x, y + r.y, z + r.z, w + r.w}; }
    constexpr Vec4 operator-(const Vec4& r) const noexcept { return {x - r.x, y - r.y, z - r.z, w - r.w}; }
    constexpr Vec4 operator-() const noexcept { return {-x, -y, -z, -w}; }

    constexpr Vec4& operator+=(const Vec4& r) noexcept { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
    constexpr Vec4& operator-=(const Vec4& r) noexcept { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }

    // scalar ops
    constexpr Vec4 operator*(float s) const noexcept { return {x * s, y * s, z * s, w * s}; }
    constexpr Vec4 operator/(float s) const noexcept { return {x / s, y / s, z / s, w / s}; }
    constexpr Vec4& operator*=(float s) noexcept { x *= s; y *= s; z *= s; w *= s; return *this; }
    constexpr Vec4& operator/=(float s) noexcept { x /= s; y /= s; z /= s; w /= s; return *this; }

    friend constexpr Vec4 operator*(float s, const Vec4& v) noexcept { return {v.x * s, v.y * s, v.z * s, v.w * s}; }

};

// Matrix4x4
struct Mat4 {
    float m[16];  // column-major (OpenGL style)

    constexpr Mat4() : m{0} {}
    constexpr explicit Mat4(float diag) : m{0} {
        m[0] = m[5] = m[10] = m[15] = diag;
    }

    // prefer Math::Mat4(1.f)
    static constexpr Mat4 identity() { return Mat4(1.f); }

    constexpr float& operator()(int row, int col) { return m[col * 4 + row]; }
    constexpr float  operator()(int row, int col) const { return m[col * 4 + row]; }

    constexpr Mat4 operator+(const Mat4& r) const {
        Mat4 res;
        for (int i = 0; i < 16; i++) res.m[i] = m[i] + r.m[i];
        return res;
    }
    constexpr Mat4 operator-(const Mat4& r) const {
        Mat4 res;
        for (int i = 0; i < 16; i++) res.m[i] = m[i] - r.m[i];
        return res;
    }
    constexpr Mat4 operator*(float s) const {
        Mat4 res;
        for (int i = 0; i < 16; i++) res.m[i] = m[i] * s;
        return res;
    }

    constexpr Mat4 operator*(const Mat4& r) const {
        Mat4 res;
        for (int col = 0; col < 4; col++) {
            for (int row = 0; row < 4; row++) {
                float sum = 0.f;
                for (int k = 0; k < 4; k++)
                    sum += (*this)(row, k) * r(k, col);
                res(row, col) = sum;
            }
        }
        return res;
    }

    constexpr Mat4 operator*=(const Mat4 r) {
        *this = *this * r;
        return *this;
    }

    constexpr Vec3 multiplyPoint(const Vec3& v) const {
        float x = v.x * (*this)(0, 0) + v.y * (*this)(0, 1) + v.z * (*this)(0, 2) + (*this)(0, 3);
        float y = v.x * (*this)(1, 0) + v.y * (*this)(1, 1) + v.z * (*this)(1, 2) + (*this)(1, 3);
        float z = v.x * (*this)(2, 0) + v.y * (*this)(2, 1) + v.z * (*this)(2, 2) + (*this)(2, 3);
        float w = v.x * (*this)(3, 0) + v.y * (*this)(3, 1) + v.z * (*this)(3, 2) + (*this)(3, 3);
        if (std::fabs(w) > 1e-6f) {
            float invW = 1.f / w;
            return {x * invW, y * invW, z * invW};
        }
        return {x, y, z};
    }

    constexpr Vec3 multiplyVector(const Vec3& v) const {
        // no translation
        return {
            v.x * (*this)(0, 0) + v.y * (*this)(0, 1) + v.z * (*this)(0, 2),
            v.x * (*this)(1, 0) + v.y * (*this)(1, 1) + v.z * (*this)(1, 2),
            v.x * (*this)(2, 0) + v.y * (*this)(2, 1) + v.z * (*this)(2, 2)
        };
    }

    // factory functions
    static constexpr Mat4 translate(const Vec3& t) {
        Mat4 m = identity();
        m(0, 3) = t.x;
        m(1, 3) = t.y;
        m(2, 3) = t.z;
        return m;
    }

    static constexpr Mat4 scale(const Vec3& s) {
        Mat4 m{};
        m(0, 0) = s.x; m(1, 1) = s.y; m(2, 2) = s.z; m(3, 3) = 1.f;
        return m;
    }

    static Mat4 rotateX(float radians) {
        Mat4 m = identity();
        float c = std::cos(radians), s = std::sin(radians);
        m(1, 1) = c; m(1,2) = -s;
        m(2, 1) = s; m(2,2) = c;
        return m;
    }

    static Mat4 rotateY(float radians) {
        Mat4 m = identity();
        float c = std::cos(radians), s = std::sin(radians);
        m(0, 0) = c; m(0,2) = s;
        m(2, 0) = -s; m(2,2) = c;
        return m;
    }

    static Mat4 rotateZ(float radians) {
        Mat4 m = identity();
        float c = std::cos(radians), s = std::sin(radians);
        m(0, 0) = c; m(0, 1) = -s;
        m(1, 0) = s; m(1, 1) = c;
        return m;
    }

    static Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar, bool depthZeroToOne = false) {
        Mat4 m{};
        const float rl = right - left;
        const float tb = top - bottom;
        const float fn = zFar - zNear;

        m(0, 0) = 2.f / rl;
        m(1, 1) = 2.f / tb;

        if (!depthZeroToOne) {
            // OpenGL style NDC: z in [-1, 1]
            m(2, 2) = -2.f / fn;
            m(0, 3) = -(right + left) / rl;
            m(1, 3) = -(top + bottom) / tb;
            m(2, 3) = -(zFar + zNear) / fn;
            m(3, 3) = 1.f;
        } else {
            // Vulkan/DirectX style NDC: z in [0, 1]
            m(2, 2) = 1.f / fn;
            m(0, 3) = -(right + left) / rl;
            m(1, 3) = -(top + bottom) / tb;
            m(2, 3) = -zNear / fn;
            m(3, 3) = 1.f;
        }
        return m;
    }

    static Mat4 orthoCentered(float width, float height, float zNear, float zFar, bool depthZeroToOne = false) {
        float hw = width  * 0.5f;
        float hh = height * 0.5f;
        return ortho(-hw, hw, -hh, hh, zNear, zFar, depthZeroToOne);
    }
};
}  // namespace SmallGraphicsLayer::Math
