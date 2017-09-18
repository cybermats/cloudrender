#pragma once

#include <random>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "imaterial.h"
#include "ray.h"
#include "intersection.h"
#include "vecmath.h"
#include "config.h"
#include "color.h"
#include "logger.h"

class matte_material : public imaterial
{
 private:
  color_t _color;

  std::mt19937 _gen;
  std::uniform_real_distribution<> _u1;
  std::uniform_real_distribution<> _u2;

 public:
 matte_material(const color_t& color)
   : _color(color)
    , _gen(std::random_device()())
    , _u1(0, 1)
    , _u2(0, 1)
    {}

  virtual ray_t shade(const intersection_t& i) override {
    auto unit = vec3f(0, 0, 1);

    auto rot_mat = find_rotation_matrix(unit, i.normal);

    // generate new vector

    auto u1 = _u1(_gen);
    auto u2 = _u2(_gen);
    const float r = sqrt(u1);
    const float theta = 2 * M_PI * u2;
    const float x = r * cos(theta);
    const float y = r * sin(theta);
    const float z = sqrt(std::max(0.0, 1 - u1));
    auto dir = vec3f(x, y, z);


    assert(std::abs(dir.length() - 1.) < config::ERR);
    dir = rot_mat * dir;
#ifndef NDEBUG
    auto tmp = std::abs(dir.length() - 1.);
    if (tmp > config::ERR) {
      LOG_ERROR << "i.t: " << i.t;
      LOG_ERROR << "i.tri: " << *(i.tri);
      LOG_ERROR << "i.normal: " << i.normal;
      LOG_ERROR << "i.normal.length: " << i.normal.length();
      LOG_ERROR << "i.ray: " << i.ray;
      LOG_ERROR << "i.position: " << i.position;
      LOG_ERROR << "rot_mat: " << rot_mat;
    }
#endif
    assert(std::abs(dir.length() - 1.) < config::ERR);
    return ray_t(i.position + dir * config::ERR,
	       dir,
	       i.ray->color() * _color,
	       i.ray->age()+1);
  }
    
};
