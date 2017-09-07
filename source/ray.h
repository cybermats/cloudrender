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
  vec3f _inv_direction;
  color _color;
  int _age;
  int _sign[3];

 public:
 ray(const vec3f& origin, const vec3f& direction, const color& color, int age = 0) 
   : _origin(origin)
    , _direction(direction)
    , _inv_direction(1/direction.x, 1/direction.y, 1/direction.z)
    , _color(color)
    , _age(age)
  {
    auto len = std::abs(_direction.length() - 1.);
    if (len > config::ERR) {
      std::cout << "origin: " << origin << std::endl;
      std::cout << "dir: " << direction << std::endl;
    }
			
    assert(std::abs(_direction.length() - 1.) < config::ERR);


    _sign[0] = (_inv_direction.x < 0);
    _sign[1] = (_inv_direction.y < 0);
    _sign[2] = (_inv_direction.z < 0);
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

  const vec3f& inv_direction() const {
    return _inv_direction;
  }

  const int* sign() const {
    return _sign;
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
