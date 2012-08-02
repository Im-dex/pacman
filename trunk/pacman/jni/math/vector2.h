#pragma once

#include "base.h"
#include "math.h"

#include <array>

namespace Pacman {
namespace Math {

template <typename T>
class Vector2
{
public:

	PACMAN_CHECK_ARITHMETIC_TYPE;

    static const Vector2<T> kZero;
    static const Vector2<T> kXAxis;
    static const Vector2<T> kYAxis;

    explicit Vector2() = default;
    explicit Vector2(const T x, const T y);
    explicit Vector2(const T xy);
    explicit Vector2(const T* data);
    Vector2(const Vector2<T>& other);
    Vector2(Vector2<T>&& other);
    ~Vector2() = default;

    Vector2<T>& operator= (const Vector2<T>& other);
    Vector2<T>& operator= (Vector2<T>&& other);

    bool operator== (const Vector2<T>& other) const;
    bool operator!= (const Vector2<T>& other) const;

    Vector2<T> operator+ (const Vector2<T>& other) const;
    Vector2<T> operator- () const;
    Vector2<T> operator- (const Vector2<T>& other) const;
    Vector2<T> operator* (const Vector2<T>& other) const;
    Vector2<T> operator/ (const Vector2<T>& other) const;

    Vector2<T>& operator+= (const Vector2<T>& other);
    Vector2<T>& operator-= (const Vector2<T>& other);
    Vector2<T>& operator*= (const Vector2<T>& other);
    Vector2<T>& operator/= (const Vector2<T>& other);

    Vector2<T> operator+ (const T xy) const;
    Vector2<T> operator- (const T xy) const;
    Vector2<T> operator* (const T xy) const;
    Vector2<T> operator/ (const T xy) const;

    Vector2<T>& operator+= (const T xy);
    Vector2<T>& operator-= (const T xy);
    Vector2<T>& operator*= (const T xy);
    Vector2<T>& operator/= (const T xy);

    T DotProduct(const Vector2<T>& other) const;

    // dot product operator
    T operator& (const Vector2<T>& other) const;

    // vector length
    T Length() const;

    T GetX() const;
    T GetY() const;

    void SetX(const T x);
    void SetY(const T y);

    Vector2<T>& Reverse();

private:

    union
    {
        struct
        {
            T mX;
            T mY;
        };
        std::array<T, 2> mData;
    };
};

template <typename T>
FORCEINLINE Vector2<T> operator* (const T val, const Vector2<T>& vec2)
{
    return vec2 * val;
}

typedef Vector2<float>  Vector2f;
typedef Vector2<double> Vector2d;

} // Math namespace
} // Pacman namespace

#include "vector2.inl"
