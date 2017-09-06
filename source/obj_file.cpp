#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#include "obj_file.h"

#include "vecmath.h"
#include "triangle.h"
#include "imaterial.h"
#include "color.h"
#include "matte_material.h"


namespace obj_reader
{
  static vec3f& get_vec(std::vector<vec3f>& vertices, int i) {
    assert(i != 0);
    int idx = i - 1;
    if (i < 0) {
      idx = vertices.size() + i;
    }
    if (idx < 0 || idx >= vertices.size())
      std::cout << "idx: " << idx << ", i: " << i << ", size(): "
		<< vertices.size() <<  std::endl;
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

  void read_mtl_file(std::string dir, std::string filename, scene& sc) {
    std::string filepath = dir + filename;
    std::cout << "reading mtl file: " << filepath << std::endl;

    std::ifstream stream(filepath, std::ifstream::in);
    std::cout << "stream: " << stream.is_open() << std::endl;

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
	  std::cout << "newmtl: " << mtl_name << std::endl;
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
	std::cout << "Kd: " << c << std::endl;
      }
    }
    if (!first) {
      std::cout << "newmtl: " << mtl_name << std::endl;
      sc.material().add_material(mtl_name, new matte_material(c));
    }
  }


  void read_obj_file(std::string dir, std::string filename, scene& sc) {
    std::string filepath = dir + filename;
    
    std::ifstream stream(filepath, std::ifstream::in);

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
      else if (items[0] == "mtllib") {
	std::cout << "mtllib: " << items[1] << std::endl;
	read_mtl_file(dir, items[1], sc);
      }
      else if (items[0] == "usemtl") {
	std::cout << "usemtl: " << items[1] << std::endl;
	material = sc.material().lookup_material(items[1]);
      }
    }
  }
}
