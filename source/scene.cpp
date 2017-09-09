#include "scene.h"

#include "kd_tree.h"

intersection scene::intersect(ray& r)
{
  auto t_min = std::numeric_limits<float>::max();
  auto u = 0.f;
  auto v = 0.f;
#ifdef NO_ACCELERATOR  
  auto hit = (triangle*)nullptr;
  for(auto& tri: _triangles) {
    auto t = tri.intersect(r, u, v);
    if (t > 0 && t < t_min) {
      t_min = t;
      hit = &tri;
    }
  }
#else

  auto hit = _tree.intersect(r, t_min, u, v);
#endif

  auto normal = vec3f();
  auto position = vec3f();
  if (hit) {
    normal = hit->normal(u, v);
    position = r.origin() + r.direction() * t_min;
  } else {
    t_min = -1;
  }

  return intersection {
    t_min,
      hit,
      normal,
      &r,
      position
      };
}

void scene::add_triangle(const triangle& t) {
#ifdef NO_ACCELERATOR
  _triangles.push_back(t);
#else
  _tree.add_triangle(new triangle(t));
#endif
}
/*
void scene::add_camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
		       float object_dist, float image_dist, radiance_buffer* rb) {
  _cam = std::unique_ptr<camera>(new camera(position, up, lookat,
					    object_dist, image_dist, rb));
  _cam->setup_scene(*this);
}
*/
void scene::add_camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
		       float focal, float hfov, float fstop, radiance_buffer* rb) {
  _cam = std::unique_ptr<camera>(new camera(position, up, lookat,
					    focal, hfov, fstop, rb));
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
