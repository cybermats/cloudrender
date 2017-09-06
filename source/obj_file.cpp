#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#include "obj_file.h"

#include "vecmath.h"
#include "triangle.h"
#include "imaterial.h"


namespace obj_reader
{
    static vec3f& get_vec(std::vector<vec3f>& vertices, int i) {
    assert(i != 0);
    int idx = i - 1;
    if (i < 0) {
      idx = vertices.size() + idx;
    }
    assert(idx >= 0);
    assert(idx < vertices.size());
    return vertices.at(i);
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
			imaterial* material)
  {
    assert(items.size() >= 4);
    assert(material);
    auto i1 = std::stoi(items[1]);
    auto i2 = std::stoi(items[2]);
    auto i3 = std::stoi(items[3]);
    sc.add_triangle(triangle(get_vec(vertices, i1),
			     get_vec(vertices, i2),
			     get_vec(vertices, i3),
			     material));
    if (items.size() >= 5) {
      auto i4 = std::stoi(items[4]);
      sc.add_triangle(triangle(get_vec(vertices, i1),
			       get_vec(vertices, i3),
			       get_vec(vertices, i4),
			       material));
    }
  }


  void read_obj_file(std::string filename, scene& sc) {
    
    std::ifstream stream(filename, std::ifstream::in);

    std::vector<vec3f> vertices;
    imaterial* material = nullptr;
    
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
	read_face(sc, vertices, items, material);
      }
    }
  }
}
