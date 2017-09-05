#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <boost/algorithm/string.hpp>

#include "vecmath.h"
#include "triangle.h"
#include "imaterial.h"
#include "scene.h"

class obj_file
{
 private:
  std::ifstream _stream;
  imaterial *_material;
  std::vector<vec3f> _vertices;
  std::vector<triangle> _triangles;
  

 public:
  obj_file(std::string filename, imaterial *material)
    : _stream(filename, std::ifstream::in)
    , _material(material)
  {}

  vec3f& get_vec(int idx) {
    if (idx < 0) {
      assert(idx < _vertices.size());
      return _vertices[_vertices.size() - idx];
    }
    assert(idx < _vertices.size());
    return _vertices[idx];
  }

  void parse()
  {
    auto del = boost::is_any_of(" ");
    
    for (std::string line; std::getline(_stream, line);) {
      std::vector<std::string> items;
      boost::split(items, line, del);
      if (items.empty())
	continue;
      if (items[0] == "v") {
	assert(items.size() > 4);
	auto vec = vec3f(std::stof(items[1]),
			 std::stof(items[2]),
			 std::stof(items[3]));
	_vertices.push_back(vec);
      }
      else if (items[0] == "f") {
	assert(items.size() > 4);
	int i1 = std::stoi(items[1]);
	int i2 = std::stoi(items[2]);
	int i3 = std::stoi(items[3]);
	auto& v1 = get_vec(i1);
	auto& v2 = get_vec(i2);
	auto& v3 = get_vec(i3);
	auto tri = triangle(v1, v2, v3, _material);
	_triangles.push_back(tri);
      }
    }
  }

  void add_to_scene(scene& sc) {
    for(auto& tri : _triangles) {
      sc.add_triangle(tri);
    }
  }
  
  
};
