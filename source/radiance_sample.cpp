#include "radiance_sample.h"

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
