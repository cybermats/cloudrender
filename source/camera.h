#pragma once

#include <iostream>
#include <algorithm>

#include "ray.h"
#include "intersection.h"
#include "sampler.h"
#include "scene.h"
//#include "imagebuffer.h"
#include "radiance_sample.h"


class camera : public imaterial
{
 private:
  int _counter;
  float _f;
  float _do;
  float _di;
  vec3f _position;
  vec3f _view_dir;
  vec3f _u;
  vec3f _v;
  //  imagebuffer *_ib;
  radiance_buffer *_rb;

 public:
 camera(vec3f position, vec3f up, vec3f lookat, float object_dist, float image_dist,
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
        
    std::cout << "cam pos: " << _position << std::endl;
    std::cout << "cam viewDir: " << _view_dir << std::endl;
    std::cout << "cam v: " << _v << std::endl;
    std::cout << "cam u: " << _u << std::endl;
    std::cout << "cam f: " << _f << std::endl;
    std::cout << "cam do: " << _do << std::endl;
    std::cout << "cam di: " << _di << std::endl;
  }

  void setup_scene(scene& s) {
    auto v0 = _position - (0.1f * _v) - (0.1f * _u);
    auto v1 = _position - (0.1f * _v) + (0.1f * _u);
    auto v2 = _position + (0.1f * _v) - (0.1f * _u);
    auto v3 = _position + (0.1f * _v) + (0.1f * _u);

    auto t1 = triangle(v0, v2, v1, this);
    auto t2 = triangle(v1, v2, v3, this);

    std::cout << "t1.normal: " << t1.normal() << std::endl;
    std::cout << "t2.normal: " << t2.normal() << std::endl;
        
    s.add_triangle(t1);
    s.add_triangle(t2);
  }

  bool lens_equation(const intersection& i, vec2f& uv)
  {
    auto dir = -i.ray->direction();
    if (dir.dot(_view_dir) < 0)
      return false;

    //	std::cout << "dir: " << dir << std::endl;
    auto v_dir_dot = dir.dot(_v);
    auto v_negative = v_dir_dot < 0;
    auto u_dir_dot = dir.dot(_u);
    auto u_negative = u_dir_dot < 0;
	

    auto v_dir = (dir - (u_dir_dot * _u)).normalize();
    auto u_dir = (dir - (v_dir_dot * _v)).normalize();

    //	std::cout << "v_dir: " << v_dir << std::endl;
    //	std::cout << "u_dir: " << u_dir << std::endl;

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
    /*
      std::cout << "v_cos: " << v_cos << std::endl;
      std::cout << "v_sin: " << v_sin << std::endl;
      std::cout << "v_tan: " << v_tan << std::endl;

      std::cout << "u_cos: " << u_cos << std::endl;
      std::cout << "u_sin: " << u_sin << std::endl;
      std::cout << "u_tan: " << u_tan << std::endl;
    */
    auto pos = i.position - _position;

    //	std::cout << "pos: " << pos << std::endl;

    auto v_pos = pos.dot(_v);
    auto u_pos = pos.dot(_u);

    //	std::cout << "v_pos: " << v_pos << std::endl;
    //	std::cout << "u_pos: " << u_pos << std::endl;

    //	auto v_ho = v_pos - _do * v_tan;
    auto v_hi = _di * v_pos / _do - _di * v_tan;

    //	auto u_ho = u_pos - _do * u_tan;
    auto u_hi = _di * u_pos / _do - _di * u_tan;
    /*
      std::cout << "v_ho: " << v_ho << std::endl;
      std::cout << "v_hi: " << v_hi << std::endl;
      std::cout << "u_ho: " << u_ho << std::endl;
      std::cout << "u_hi: " << u_hi << std::endl;
    */

    if (u_hi < -0.1 || u_hi > 0.1)
      return false;
    if (v_hi < -0.1 || v_hi > 0.1)
      return false;
    /*
    if ((u_hi < 0.002 && u_hi > -0.002) &&
        (v_hi < 0.002 && v_hi > -0.002))
      return false;
    */
    uv.x = -u_hi;
    uv.y = -v_hi;
	
    return true;
  }

  virtual ray shade(const intersection& i) 
  {
    vec2f uv;
    if (!lens_equation(i, uv))
      return ray();
    //    _rb->add(uv, i.ray->color(), i.ray->age());
    _rb->emplace_back(uv, i.ray->color(), i.ray->age());
    /*
    auto x = (int)(((uv.x + 0.1) / 0.2) * _ib->width());
    auto y = (int)(((uv.y + 0.1) / 0.2) * _ib->height());
    //    std::cout << "uv: " << uv << ", x: " << x << ", y: " << y << std::endl;

    x = std::max(std::min(x, (int)_ib->width()-1), 0);
    y = std::max(std::min(y, (int)_ib->height()-1), 0);
    

    _ib->inc(x, y, i.ray->color());
    */  
    ++_counter;
    return ray();
  }

  int counter() {
    return _counter;
  }

};
