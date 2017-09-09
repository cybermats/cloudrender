#include <iostream>
#include <algorithm>

#include "camera.h"
#include "intersection.h"
#include "scene.h"

#include "logger.h"

/*
camera::camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
	       float object_dist, float image_dist,
	       radiance_buffer* rb)
  : _counter(0)
  , _do(object_dist), _di(image_dist)
  , _rb(rb)
{
  _position = position;
  _view_dir = (lookat - position).normalize();
  _u = _view_dir.cross(up).normalize();
  _v = _u.cross(_view_dir).normalize();


  _f = 1.f / (1.f/_do + 1.f/_di);
}
*/
camera::camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
	       float focal, float hfow, float fstop,
	       radiance_buffer* rb)
  : _counter(0)
  , _f(focal)
  , _aperture_diameter(focal / fstop)
  , _rb(rb)
{
  _position = position;
  auto viewing = lookat - position;
  auto object_dist = viewing.length();
  _view_dir = viewing * (1/object_dist);

  _f = focal;
  _do = object_dist;
  _di = 1.f / (1.f/_f - 1.f/_do);

  _h = (_di * hfow ) / ( 2 * _do);
  
  _u = _view_dir.cross(up).normalize();
  _v = _u.cross(_view_dir).normalize();
}

void camera::setup_scene(scene& s) {
  /*  
  auto v0 = _position - (0.1f * _v) - (0.1f * _u);
  auto v1 = _position - (0.1f * _v) + (0.1f * _u);
  auto v2 = _position + (0.1f * _v) - (0.1f * _u);
  auto v3 = _position + (0.1f * _v) + (0.1f * _u);
  */
  auto a = _aperture_diameter / 2;
  
  auto v0 = _position - (a * _v) - (a * _u);
  auto v1 = _position - (a * _v) + (a * _u);
  auto v2 = _position + (a * _v) - (a * _u);
  auto v3 = _position + (a * _v) + (a * _u);

  auto t1 = triangle(v0, v2, v1, this);
  auto t2 = triangle(v1, v2, v3, this);

  s.add_triangle(t1);
  s.add_triangle(t2);
}

bool camera::lens_equation(const intersection& i, vec2f& uv)
{
  auto dir = -i.ray->direction();
  if (dir.dot(_view_dir) < 0)
    return false;

  auto v_dir_dot = dir.dot(_v);
  auto v_negative = v_dir_dot < 0;
  auto u_dir_dot = dir.dot(_u);
  auto u_negative = u_dir_dot < 0;
	

  auto v_dir = (dir - (u_dir_dot * _u)).normalize();
  auto u_dir = (dir - (v_dir_dot * _v)).normalize();

  auto v_cos = -v_dir.dot(_view_dir);
  auto v_sin = v_dir.cross(_view_dir).length();
  if (v_negative)
    v_sin = -v_sin;
  auto v_tan = v_sin/v_cos;

  auto u_cos = -u_dir.dot(_view_dir);
  auto u_sin = _view_dir.cross(u_dir).length();
  if (u_negative)
    u_sin = -u_sin;
  auto u_tan = u_sin/u_cos;

  auto pos = i.position - _position;

  auto v_pos = pos.dot(_v);
  auto u_pos = pos.dot(_u);

  auto v_hi = _di * v_pos / _do - _di * v_tan;
  auto u_hi = _di * u_pos / _do - _di * u_tan;

  if (u_hi < -_h || u_hi > _h)
    return false;
  if (v_hi < -_h || v_hi > _h)
    return false;

  uv.x = (-u_hi)/_h;
  uv.y = (-v_hi)/_h;
	
  return true;
}

ray camera::shade(const intersection& i) 
{
  vec2f uv;
  if (!lens_equation(i, uv))
    return ray();
  _mtx.lock();
  _rb->emplace_back(uv, i.ray->color(), i.ray->age());
  _mtx.unlock();
  ++_counter;
  return ray();
}
