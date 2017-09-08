#pragma once

#include <random>
#define _USE_MATH_DEFINES
#include <cmath>


#include "ilight.h"
#include "color.h"
#include "triangle.h"

class trilight : public ilight
{
 private:
  color _color;
  float _relevance;
  std::mt19937 _gen;
  std::uniform_real_distribution<float> _u;
  std::uniform_real_distribution<float> _v;
  std::uniform_real_distribution<float> _theta;
  std::uniform_real_distribution<float> _phi;
  

  vec3f _position;
  vec3f _v1;
  vec3f _v2;
  
 public:

  trilight(const triangle& tri, const color& color, float relevance)
    : _color(color), _relevance(relevance)
    , _gen(std::random_device()())
    , _u(0, 1)
    , _v(0, 1)
    , _theta(0, 2 * M_PI)
    , _phi(-1, 1)
    , _position(tri.v0())
    , _v1(tri.v1() - tri.v0())
    , _v2(tri.v2() - tri.v0())
  {}

 
  virtual ray generate_ray() override {
    auto u = _u(_gen);
    auto v = _v(_gen);

    if (u + v > 1.f) {
      u = 1.f - u;
      v = 1.f - v;
    }
    assert(u + v > 0);
    assert(u + v <= 1.f);


    auto theta = _theta(_gen);
    auto phi = _phi(_gen);
    auto r = sqrt(1.f - phi*phi);

    auto dir = vec3f(r * cos(theta),
		     r * sin(theta),
		     phi);

    auto orig = _position + (u * _v1) + (v * _v2) + (dir * config::ERR);

    return ray(orig, dir, _color);
  }

  virtual float relevance() const override {
    return _relevance;
  }
};
