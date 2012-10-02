#pragma once

#include <array>

#include "base.h"
#include "math.h"

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

    typedef T value_t;

    explicit Vector2() = default;
    explicit Vector2(const T x, const T y);
    explicit Vector2(const T xy);
    explicit Vector2(const T* data);
    Vector2(const Vector2<T>& other);
    ~Vector2() = default;

    Vector2<T>& operator= (const Vector2<T>& other);

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

    Vector2<T> Reverse() const;

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
const Vector2<T> Vector2<T>::kZero  = Vector2<T>(T(0), T(0));

template <typename T>
const Vector2<T> Vector2<T>::kXAxis = Vector2<T>(T(1), T(0));

template <typename T>
const Vector2<T> Vector2<T>::kYAxis = Vector2<T>(T(0), T(1));

template <typename T>
FORCEINLINE Vector2<T> operator* (const T val, const Vector2<T>& vec2)
{
    return vec2 * val;
}

typedef Vector2<float>  Vector2f;
typedef Vector2<double> Vector2d;
typedef Vector2<size_t> Vector2s;

} // Math namespace
} // Pacman namespace

#include "vector2.inl"
