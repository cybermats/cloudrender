#include <algorithm>

#include "color.h"

color_t operator+(const color_t& left, const color_t& right) {
  return color_t(left.r + right.r, left.g + right.g, left.b + right.b, std::max(left.a, right.a));
}

color_t& operator+=(color_t& left, const color_t& right) {
  left.r += right.r;
  left.g += right.g;
  left.b += right.b;
  left.a += right.a;
  return left;
}

color_t operator*(const color_t& l, const color_t& r) {
  return color_t(l.r * r.r, l.g * r.g, l.b * r.b, l.a * r.a);
}

color_t operator*(const color_t& c, float v) {
  return color_t(c.r * v, c.g * v, c.b * v, c.a);
}

color_t operator*(float v, const color_t& c) {
  return color_t(c.r * v, c.g * v, c.b * v, c.a);
}
