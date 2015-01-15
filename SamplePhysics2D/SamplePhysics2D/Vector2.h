#pragma once

struct Vector2
{
    Vector2() : x(0), y(0) {}
    Vector2(int x, int y) : x((float)x), y((float)y) {}
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(const Vector2& other) : x(other.x), y(other.y) {}

    // Negate
    Vector2 operator- ()
    {
        return Vector2(-x, -y);
    }

    // Assign
    Vector2& operator= (const Vector2& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    Vector2& operator+= (const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-= (const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // Lengths
    float LengthSq() const
    {
        return x * x + y * y;
    }

    float Length() const
    {
        return sqrtf(LengthSq());
    }

    // Normalization
    void Normalize()
    {
        float invLen = Length();
        assert(invLen != 0);
        invLen = 1.0f / invLen;
        x *= invLen;
        y *= invLen;
    }

    Vector2 Normalized() const
    {
        Vector2 n(*this);
        n.Normalize();
        return n;
    }

    float x, y;
};

inline Vector2 operator+ (const Vector2& a, const Vector2& b)
{
    return Vector2(a.x + b.x, a.y + b.y);
}

inline Vector2 operator- (const Vector2& a, const Vector2& b)
{
    return Vector2(a.x - b.x, a.y - b.y);
}

inline Vector2 operator* (float s, const Vector2& a)
{
    return Vector2(a.x * s, a.y * s);
}

inline Vector2 operator* (const Vector2& a, float s)
{
    return Vector2(a.x * s, a.y * s);
}

inline Vector2 operator/ (const Vector2& a, float s)
{
    return Vector2(a.x / s, a.y / s);
}

inline float Dot(const Vector2& a, const Vector2& b)
{
    return a.x * b.x + a.y * b.y;
}

// Finds the length of the resulting vector from crossing the 2D vectors
inline float Cross(const Vector2& a, const Vector2& b)
{
    return a.x * b.y - a.y * b.x;
}

// 2D Cross (perp) product with a given length
inline Vector2 Cross(const Vector2& a, float s)
{
    return Vector2(s * a.y, -s * a.x);
}

// 2D Cross (perp) product with a given length
inline Vector2 Cross(float s, const Vector2& a)
{
    return Vector2(-s * a.y, s * a.x);
}
