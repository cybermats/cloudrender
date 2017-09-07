#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "obj_file.h"

#include "vecmath.h"
#include "triangle.h"
#include "imaterial.h"
#include "color.h"
#include "matte_material.h"
#include "trilight.h"


namespace obj_reader
{
  static vec3f& get_vec(std::vector<vec3f>& vertices, int i) {
    assert(i != 0);
    int idx = i - 1;
    if (i < 0) {
      idx = vertices.size() + i;
    }
    assert(idx >= 0);
    assert(idx < vertices.size());
    return vertices.at(idx);
  }
  
  static void read_vertex(std::vector<vec3f>& vertices,
			  std::vector<std::string>& items)
  {
    assert(items.size() >= 4);
    vertices.emplace_back(std::stof(items[1]),
			  std::stof(items[2]),
			  std::stof(items[3]));
  }

  static void read_face(scene& sc,
			std::vector<vec3f>& vertices,
			std::vector<std::string>& items,
			imaterial* material,
			color *emissive)
  {
    assert(items.size() >= 4);
    assert(material);
    auto i1 = std::stoi(items[1]);
    auto i2 = std::stoi(items[2]);
    auto i3 = std::stoi(items[3]);
    auto tri = triangle(get_vec(vertices, i1),
			get_vec(vertices, i2),
			get_vec(vertices, i3),
			material);
    sc.add_triangle(tri);
    if (emissive != nullptr) {
      sc.lightsource().add_light(new trilight(tri, *emissive, 1.f));
    }
  
    if (items.size() >= 5) {
      auto i4 = std::stoi(items[4]);
      auto tri = triangle(get_vec(vertices, i1),
		          get_vec(vertices, i3),
		          get_vec(vertices, i4),
		          material);
      sc.add_triangle(tri);
      if (emissive != nullptr) {
	sc.lightsource().add_light(new trilight(tri, *emissive, 1.f));
      }
    }
  }

  void read_mtl_file(const std::string& dir, const std::string& filename,
		     scene& sc, std::map<std::string, color>& emissives) {
    std::string filepath = dir + filename;

    std::ifstream stream(filepath, std::ifstream::in);

    std::string mtl_name;
    color c;
    bool first = true;

    for (std::string line; std::getline(stream, line);) {
      std::vector<std::string> items;
      boost::trim(line);
      boost::split(items, line, boost::is_any_of(" \t"),
		   boost::algorithm::token_compress_on);

      if (items.empty()) {
	continue;
      }

      if (items[0] == "newmtl") {
	if (!first) {
	  sc.material().add_material(mtl_name, new matte_material(c));
	}
	mtl_name = items[1];
	c = color(0, 0, 0);
	first = false;
      }
      else if(items[0] == "Kd") {
	assert(!first);
	assert(items.size() >= 4);
	c = color(std::stof(items[1]),
		  std::stof(items[2]),
		  std::stof(items[3]));
      }
      else if(items[0] == "Ke") {
	assert(!first);
	assert(items.size() >= 4);
	auto ec = color(std::stof(items[1]),
			std::stof(items[2]),
			std::stof(items[3]));
	if (ec.r > 0 && ec.g > 0 && ec.b > 0) {
	  emissives[mtl_name] = ec;
	}
      }
      
    }
    if (!first) {
      sc.material().add_material(mtl_name, new matte_material(c));
    }
  }


  void read_obj_file(const std::string& dir, const std::string& filename,
		     scene& sc, bool allow_lights) {
    std::string filepath = dir + filename;
    
    std::ifstream stream(filepath, std::ifstream::in);

    std::vector<vec3f> vertices;
    imaterial* material = nullptr;
    std::map<std::string, color> emissives;
    color *emissive = nullptr;
    
    for (std::string line; std::getline(stream, line);) {
      
      std::vector<std::string> items;
      boost::split(items, line, boost::is_any_of(" \t"),
		   boost::algorithm::token_compress_on);

      if (items.empty()) {
	continue;
      }

      if (items[0] == "v") {
	read_vertex(vertices, items);
      }
      else if (items[0] == "f") {
	read_face(sc, vertices, items, material, emissive);
      }
      else if (items[0] == "mtllib") {
	read_mtl_file(dir, items[1], sc, emissives);
      }
      else if (items[0] == "usemtl") {
	material = sc.material().lookup_material(items[1]);
	auto e_it = emissives.find(items[1]);
	if (allow_lights && e_it != emissives.end()) {
	  emissive = &(e_it->second);
	}
	else {
	  emissive = nullptr;
	}
      }
    }
  }
}
