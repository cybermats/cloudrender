#pragma once

#include <iostream>

#include "vecmath.h"

struct radiance_sample
{
  radiance_sample(){}

  radiance_sample(const vec2f& pos, const color& c, int age)
  : plate_pos(pos)
  , color(c)
  , ray_age(age)
  {}

  
  vec2f plate_pos;
  color color;
  int ray_age;
};

std::ostream& operator<<(std::ostream& stream, const radiance_sample& sample)
{
  stream << sample.plate_pos.x << " " << sample.plate_pos.y << " "
    << sample.color.r << " " << sample.color.g << " " <<
    sample.color.b << " " << sample.color.a << " "
	 << sample.ray_age;
  return stream;
}


std::istream& operator>>(std::istream& stream, radiance_sample& sample)
{
  stream >> sample.plate_pos.x >> sample.plate_pos.y >>
    sample.color.r >> sample.color.g >> sample.color.b >> sample.color.a >>
    sample.ray_age;
  return stream;
}

using radiance_buffer = std::vector<radiance_sample>;


std::ostream& operator<<(std::ostream& stream, const radiance_buffer& buffer)
{
  for(auto& s : buffer) {
    stream << s << std::endl;
  }
  return stream;
}


std::istream& operator>>(std::istream& stream, radiance_buffer& buffer)
{
  radiance_sample s;
  while (stream >> s) {
    buffer.push_back(s);
  }
  return stream;
}
