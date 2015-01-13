#pragma once

// Simple 2x2 matrix
struct Matrix2
{
    Matrix2() {}
    Matrix2(float angle)
    {
        float c = cosf(angle);
        float s = sinf(angle);
        col1.x = c; col2.x = -s;
        col1.y = s; col2.y = c;
    }

    Matrix2(const Vector2& col1, const Vector2& col2) : col1(col1), col2(col2) {}

    void Transpose()
    {
        std::swap(col1.y, col2.x);
    }

    Matrix2 Transposed() const
    {
        return Matrix2(Vector2(col1.x, col2.x), Vector2(col1.y, col2.y));
    }

    Vector2 col1, col2;
};

inline Vector2 operator* (const Matrix2& A, const Vector2& v)
{
    return Vector2(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}

inline Matrix2 operator* (const Matrix2& A, const Matrix2& B)
{
    return Matrix2(A * B.col1, A * B.col2);
}
