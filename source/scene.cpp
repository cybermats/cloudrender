#include "scene.h"

intersection scene::intersect(ray& r)
{
  auto hit = (triangle*)nullptr;
  auto t_max = std::numeric_limits<float>::max();
  for(auto& tri: _triangles)
    {
      auto t = tri.intersect(r);
      if (t > 0 && t < t_max) {
	t_max = t;
	hit = &tri;
      }
    }

  auto normal = vec3f();
  auto position = vec3f();
  if (hit) {
    normal = hit->normal();
    position = r.origin() + r.direction() * t_max;
  } else {
    t_max = -1;
  }

  return intersection {
    t_max,
      hit,
      normal,
      &r,
      position
      };
}

void scene::add_triangle(const triangle& t) {
  _triangles.push_back(t);
}

void scene::add_camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
		       float object_dist, float image_dist, radiance_buffer* rb) {
  _cam = std::unique_ptr<camera>(new camera(position, up, lookat,
					    object_dist, image_dist, rb));
  _cam->setup_scene(*this);
}

lightsource& scene::lightsource()
{
  return _lightsource;
}

material_collection& scene::material()
{
  return _materials;
}
