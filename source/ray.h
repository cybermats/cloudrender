#pragma once

#include <cassert>
#include <cstdlib>

#include "vecmath.h"
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

std::ostream& operator<<(std::ostream& stream, const ray& r) {
  stream << "ray( origin: " << r.origin() << ", dir: " << r.direction() << ")";
  return stream;
}
