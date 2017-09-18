#include "ray.h"

std::ostream& operator<<(std::ostream& stream, const ray_t& r) {
  stream << "ray( origin: " << r.origin() << ", dir: " << r.direction() << ")";
  return stream;
}
