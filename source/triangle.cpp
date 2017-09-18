#include "triangle.h"


std::ostream& operator<<(std::ostream& stream, const triangle_t& tri) {
  stream << "triangle(v0 = " << tri._v0 << ", v1 = " << tri._v1
	 << ", v2 = " << tri._v2 << ", normal = " << tri._normal << ")";
  return stream;
}
