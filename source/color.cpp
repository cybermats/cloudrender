#include <algorithm>

#include "color.h"

color operator+(const color& left, const color& right) {
  return color(left.r + right.r, left.g + right.g, left.b + right.b, std::max(left.a, right.a));
}

color& operator+=(color& left, const color& right) {
  left.r += right.r;
  left.g += right.g;
  left.b += right.b;
  left.a += right.a;
  return left;
}

color operator*(const color& l, const color& r) {
  return color(l.r * r.r, l.g * r.g, l.b * r.b, l.a * r.a);
}

color operator*(const color& c, float v) {
  return color(c.r * v, c.g * v, c.b * v, c.a);
}

color operator*(float v, const color& c) {
  return color(c.r * v, c.g * v, c.b * v, c.a);
}
