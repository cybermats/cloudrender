#pragma once

#include <mutex>

#include "sampler.h"
#include "radiance_sample.h"
#include "ray.h"
#include "imaterial.h"

class scene_t;

class camera_t : public imaterial
{
 private:
  int _counter;
  float _f;
  float _do;
  float _di;
  float _aperture_diameter;
  float _h;
  vec3f _position;
  vec3f _view_dir;
  vec3f _u;
  vec3f _v;
  radiance_buffer *_rb;
  std::mutex _mtx;

 public:
  /*
  camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
	 float object_dist, float image_dist,
	 radiance_buffer* rb);
  */

  camera_t(const vec3f& position, const vec3f& up, const vec3f& lookat,
	 float focal, float hfov, float fstop,
	 radiance_buffer* rb);

  void setup_scene(scene_t& s);

  bool lens_equation(const intersection_t& i, vec2f& uv);

  virtual ray_t shade(const intersection_t& i);

  int counter() {
    return _counter;
  }

  float f() const {
    return _f;
  }

  float d_o() const {
    return _do;
  }

  float h() const {
    return _h;
  }

  float d_i() const {
    return _di;
  }

  vec3f position() const {
    return _position;
  }

  vec3f view_dir() const {
    return _view_dir;
  }

  vec3f u() const {
    return _u;
  }

  vec3f v() const {
    return _v;
  }

  float aperture() const {
    return _aperture_diameter;
  }

};
