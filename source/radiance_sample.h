#pragma once

#include <iostream>
#include <vector>

#include "vecmath.h"
#include "color.h"

struct radiance_sample
{
  radiance_sample(){}

radiance_sample(const vec2f& pos, const color_t& c, int age)
: plate_pos(pos)
, color(c)
    , ray_age(age)
  {}

  
  vec2f plate_pos;
  color_t color;
  int ray_age;
};

std::ostream& operator<<(std::ostream& stream, const radiance_sample& sample);

std::istream& operator>>(std::istream& stream, radiance_sample& sample);

using radiance_buffer = std::vector<radiance_sample>;

std::ostream& operator<<(std::ostream& stream, const radiance_buffer& buffer);

std::istream& operator>>(std::istream& stream, radiance_buffer& buffer);
