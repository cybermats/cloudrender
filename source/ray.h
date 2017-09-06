#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "vecmath.h"
#include "vecutil.h"
#include "color.h"
#include "config.h"

class ray
{
 private:
  vec3f _origin;
  vec3f _direction;
  color _color;
  int _age;

 public:
 ray(const vec3f& origin, const vec3f& direction, const color& color, int age = 0) 
   : _origin(origin)
    , _direction(direction)
    , _color(color)
    , _age(age)
  {
    auto len = std::abs(_direction.length() - 1.);
    if (len > config::ERR) {
      std::cout << "origin: " << origin << std::endl;
      std::cout << "dir: " << direction << std::endl;
    }
			
    assert(std::abs(_direction.length() - 1.) < config::ERR);
  }

 ray()
    : _age(config::get_config().max_ray_age)
    {}

  const vec3f& origin() const {
    return _origin;
  }

  const vec3f& direction() const {
    return _direction;
  }

  const color& color() const {
    return _color;
  }

  int age() const {
    return _age;
  }

  bool active() const {
    return _age < config::get_config().max_ray_age;
  }
};

std::ostream& operator<<(std::ostream& stream, const ray& r);
