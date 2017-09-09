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

class matte_material : public imaterial
{
 private:
  color _color;

  std::mt19937 _gen;
  std::uniform_real_distribution<> _u1;
  std::uniform_real_distribution<> _u2;

 public:
 matte_material(const color& color)
   : _color(color)
    , _gen(std::random_device()())
    , _u1(0, 1)
    , _u2(0, 1)
    {}

  virtual ray shade(const intersection& i) override {
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
      std::cout << "i.t: " << i.t << std::endl;
      std::cout << "i.tri: " << *(i.tri) << std::endl;
      std::cout << "i.normal: " << i.normal << std::endl;
      std::cout << "i.normal.length: " << i.normal.length() << std::endl;
      std::cout << "i.ray: " << i.ray << std::endl;
      std::cout << "i.position: " << i.position << std::endl;
      std::cout << "rot_mat: " << rot_mat << std::endl;
    }
#endif
    assert(std::abs(dir.length() - 1.) < config::ERR);
    return ray(i.position + dir * config::ERR,
	       dir,
	       i.ray->color() * _color,
	       i.ray->age()+1);
  }
    
};
