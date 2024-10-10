#pragma once
#include <complex>

namespace EngineCore
{
    // utility 2d vector
    template <typename T>
    struct Vector2D
    {
        T X;
        T Y;

        Vector2D() : X(static_cast<T>(0)), Y(static_cast<T>(0))
        {
        }

        Vector2D(T xVal, T yVal) : X(xVal), Y(yVal)
        {
        }

        Vector2D operator+(const Vector2D& other) const
        {
            return Vector2D(X + other.X, Y + other.Y);
        }

        Vector2D operator-(const Vector2D& other) const
        {
            return Vector2D(X - other.X, Y - other.Y);
        }

        Vector2D operator*(T scalar) const
        {
            return Vector2D(X * scalar, Y * scalar);
        }

        Vector2D operator*(const Vector2D& other) const
        {
            return Vector2D(X * other.X, Y * other.Y);
        }

        Vector2D operator/(T scalar) const
        {
            return Vector2D(X / scalar, Y / scalar);
        }
        
        bool operator==(const Vector2D& other) const {
            return X == other.X && Y == other.Y;
        }
        
        bool operator!=(const Vector2D& other) const {
            return !(*this == other);
        }

        T magnitude() const
        {
            return std::sqrt(X * X + Y * Y);
        }
    };
}
