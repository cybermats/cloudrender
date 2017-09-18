#pragma once

#include <cassert>
#include <cmath>
#include <algorithm>
#include "config.h"

template<class T>
struct vec2
{
vec2(const T& x, const T& y)
: x(x), y(y) {}

  vec2(){}

  T x;
  T y;

  T& operator[](const int index) {
    assert(index >= 0 && index < 2);
    switch(index) {
    case 0:
      return x;
    case 1:
    default:
      return y;
    }
  }

  T operator[](const int index) const {
    assert(index >= 0 && index < 2);
    switch(index) {
    case 0:
      return x;
    case 1:
    default:
      return y;
    }
  }

  vec2<T> operator+(const vec2<T>& other) const {
    return vec2<T>(x + other.x, y + other.y);
  }

  vec2<T> operator-(const vec2<T>& other) const {
    return vec2<T>(x - other.x, y - other.y);
  }

  vec2<T> operator-() const {
    return vec2<T>(-x, -y);
  }

  double length() const {
    return sqrt(x*x + y*y);
  }

  T dot(const vec2<T> other) const {
    return x * other.x + y * other.y;
  }
};

template <class VecT, class ValT>
  vec2<VecT> operator*(const vec2<VecT>& vec, const ValT& val) {
  return vec2<VecT>(vec.x * val, vec.y * val);
}

template <class VecT, class ValT>
  vec2<VecT> operator*(const ValT& val, const vec2<VecT>& vec) {
  return vec2<VecT>(vec.x * val, vec.y * val);
}

using vec2i = vec2<int>;
using vec2f = vec2<float>;





template<class T>
struct vec3
{
vec3(const T& x, const T& y, const T& z)
: x(x), y(y), z(z) {}

  vec3(){}

  T x;
  T y;
  T z;
    
  T& operator[](const int index) {
    assert(index >= 0 && index < 3);
    switch(index) {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
    default:
      return z;
    }
  }

  T operator[](const int index) const {
    assert(index >= 0 && index < 3);
    switch(index) {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
    default:
      return z;
    }
  }

  vec3<T> operator+(const vec3<T>& other) const {
    return vec3<T>(x + other.x, y + other.y, z + other.z);
  }

  vec3<T>& operator+=(const vec3<T>& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  vec3<T> operator-(const vec3<T>& other) const {
    return vec3<T>(x - other.x, y - other.y, z - other.z);
  }

  vec3<T> operator-() const {
    return vec3<T>(-x, -y, -z);
  }

  double length() const {
    return sqrt(x*x + y*y + z*z);
  }

  T dot(const vec3<T>& other) const {
    return x * other.x + y * other.y + z * other.z;
  }

  vec3<T> normalize() const {
    auto inv_len = 1.f / length();
    return *this * inv_len;
  }

  vec3<T> cross(const vec3<T>& other) const {
    return vec3<T>(
		   y * other.z - z * other.y,
		   z * other.x - x * other.z,
		   x * other.y - y * other.x
		   );
  }
};

template <class VecT, class ValT>
  vec3<VecT> operator*(const vec3<VecT>& vec, const ValT& val) {
  return vec3<VecT>(vec.x * val, vec.y * val, vec.z * val);
}

template <class VecT, class ValT>
  vec3<VecT> operator*(const ValT& val, const vec3<VecT>& vec) {
  return vec3<VecT>(vec.x * val, vec.y * val, vec.z * val);
}

template <class VecT, class ValT>
  vec3<VecT> operator/(const vec3<VecT>& vec, const ValT& val) {
  auto tmp = 1.f / val;
  return vec3<VecT>(vec.x * tmp, vec.y * tmp, vec.z * tmp);
}

using vec3i = vec3<int>;
using vec3f = vec3<float>;



struct matrix3f
{
private:
  float _cell[9];
public:

  matrix3f() {}

  matrix3f(float value) 
  {
    std::fill(_cell, _cell + 9, value);
  }

matrix3f(float aa, float ab, float ac, float ba, float bb, float bc, float ca, float cb, float cc)
: _cell{aa, ab, ac, ba, bb, bc, ca, cb, cc}
  {
  }

  struct matrix_row
  {
    matrix3f *_matrix;
    int row;

    float& operator[](int column) {
      return _matrix->_cell[row * 3 + column];
    }
  };

  struct const_matrix_row
  {
    const matrix3f *_matrix;
    int row;

    float operator[](int column) const {
      return _matrix->_cell[row * 3 + column];
    }
  };

  matrix_row operator[](int row) {
    return matrix_row{this, row};
  }

  const_matrix_row operator[](int row) const {
    return const_matrix_row{this, row};
  }

  vec3f operator* (const vec3f& vec) {
    return vec3f(
		 _cell[0] * vec.x + _cell[1] * vec.y + _cell[2] * vec.z,
		 _cell[3] * vec.x + _cell[4] * vec.y + _cell[5] * vec.z,
		 _cell[6] * vec.x + _cell[7] * vec.y + _cell[8] * vec.z            
		 );
  }

};
