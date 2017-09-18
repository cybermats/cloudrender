#pragma once

#include "vecmath.h"

struct color_t
{
color_t()
: r(0.), g(0.), b(0.), a(1.) 
  {}

color_t(float r, float g, float b, float a)
: r(r), g(g), b(b), a(a) 
  {}

color_t(float r, float g, float b)
: r(r), g(g), b(b), a(1.) 
  {}

  template <class T>
  color_t(const vec3<T>& v) 
  : r(v.x), g(v.y), b(v.z), a(1.) 
  {}

  template<class T>
  operator vec3<T>() const 
  {
    return vec3<T>(r, g, b);
  }

  float intensity() const
  {
    return (r + g + b) / 3.f;
  }

  float r;
  float g;
  float b;
  float a;
};

color_t operator+(const color_t& left, const color_t& right);
color_t& operator+=(color_t& left, const color_t& right);
color_t operator*(const color_t& l, const color_t& r);
color_t operator*(const color_t& c, float v);
color_t operator*(float v, const color_t& c);


