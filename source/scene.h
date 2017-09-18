#pragma once

#include <vector>
#include <memory>

#include "triangle.h"
#include "ray.h"
#include "vecmath.h"
#include "intersection.h"
#include "camera.h"
#include "lightsource.h"
#include "ilight.h"
#include "material_collection.h"
#include "kd_tree.h"

#define NO_ACCELERATOR

class scene_t
{
 private:
  std::unique_ptr<camera_t> _cam;
  lightsource_t _lightsource;
  material_collection _materials;
#ifdef NO_ACCELERATOR
  std::vector<triangle_t> _triangles;
#else
  kd_tree _tree;
#endif
  
 public:
  intersection_t intersect(ray_t& r);
  void add_triangle(const triangle_t& t);
  /*
  void add_camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
		  float object_dist, float image_dist, radiance_buffer* rb);
  */
  void add_camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
		  float focal, float hfov, float fstop, radiance_buffer* rb);

  
  lightsource_t& lightsource();
  material_collection& material();
  camera_t& get_camera() {
    return *_cam;
  }
  void initialize() {
#ifndef NO_ACCELERATOR
    _tree.initialize();
#endif
  }
    
};

