#pragma once

#include "vecmath.h"

struct color
{
color()
: r(0.), g(0.), b(0.), a(1.) 
  {}

color(float r, float g, float b, float a)
: r(r), g(g), b(b), a(a) 
  {}

color(float r, float g, float b)
: r(r), g(g), b(b), a(1.) 
  {}

  template <class T>
  color(const vec3<T>& v) 
  : r(v.x), g(v.y), b(v.z), a(1.) 
  {}
    

  template<class T>
  operator vec3<T>() const 
  {
    return vec3<T>(r, g, b);
  }

  float r;
  float g;
  float b;
  float a;
};

color operator+(const color& left, const color& right);
color& operator+=(color& left, const color& right);
color operator*(const color& l, const color& r);
color operator*(const color& c, float v);
color operator*(float v, const color& c);


