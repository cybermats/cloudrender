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

class scene
{
 private:
  std::unique_ptr<camera> _cam;
  std::vector<triangle> _triangles;
  lightsource _lightsource;
  material_collection _materials;
  
 public:
  intersection intersect(ray& r);
  void add_triangle(const triangle& t);
  void add_camera(const vec3f& position, const vec3f& up, const vec3f& lookat,
		  float object_dist, float image_dist, radiance_buffer* rb);
  lightsource& lightsource();
  material_collection& material();
  camera& get_camera() {
    return *_cam;
  }
};

