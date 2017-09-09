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
  static vec3f& get_vec(std::vector<vec3f>& vecs, int i) {
    assert(i != 0);
    int idx = i - 1;
    if (i < 0) {
      idx = vecs.size() + i;
    }
    assert(idx >= 0);
    assert(idx < vecs.size());
    return vecs.at(idx);
  }
  
  static void read_vertex(std::vector<vec3f>& vertices,
			  std::vector<std::string>& items)
  {
    assert(items.size() >= 4);
    vertices.emplace_back(std::stof(items[1]),
			  std::stof(items[2]),
			  std::stof(items[3]));
  }

  static void read_vertex_normal(std::vector<vec3f>& normals,
			  std::vector<std::string>& items)
  {
    assert(items.size() >= 4);
    normals.emplace_back(std::stof(items[1]),
			 std::stof(items[2]),
			 std::stof(items[3]));
  }

  static void read_vertex_texture(std::vector<vec3f>& textures,
			  std::vector<std::string>& items)
  {
    assert(items.size() >= 2);
    auto u = std::stof(items[1]);
    auto v = 0.f;
    auto w = 0.f;
    if (items.size() >= 3)
      v = std::stof(items[2]);
    if (items.size() >= 4)
      w = std::stof(items[3]);
    
    textures.emplace_back(u, v, w);
  }

  enum face_item_type {
    vertex = 1,
    texture = 2,
    normal = 4,
  };

  typedef std::map<face_item_type, int> face_item_info;

  static face_item_info parse_face_item(const std::string& item)
  {
    std::vector<std::string> parts;
    boost::split(parts, item, boost::is_any_of("/"));
    assert(!parts.empty());

    face_item_info info;
    info[face_item_type::vertex] = std::stoi(parts[0]);
    if (parts.size() >= 2 && !parts[1].empty())
      info[face_item_type::texture] = std::stoi(parts[1]);
    if (parts.size() >= 3 && !parts[2].empty())
      info[face_item_type::normal] = std::stoi(parts[2]);
    return info;
  }

  static bool validate_face_items(const std::vector<face_item_info>& face_info)
  {
    std::vector<int> types;
    types.resize(face_info.size());
    std::transform(face_info.begin(), face_info.end(), types.begin(),
		   [](const face_item_info& item_info) -> int {
		     int item = std::accumulate(item_info.begin(),
						 item_info.end(),
						 0,
						 [](int a,
						    const face_item_info::value_type& b)
						 -> int {
				       return a | b.first;
				     });
		     return item;
		   });
    auto it = std::unique(types.begin(), types.end());
    return (std::distance(types.begin(), it) == 1);
    
  }

  static void read_face(scene& sc,
			std::vector<vec3f>& vertices,
			std::vector<vec3f>& textures,
			std::vector<vec3f>& normals,
			std::vector<std::string>& items,
			imaterial* material,
			color *emissive)
  {
    assert(items.size() >= 4);
    assert(material);

    std::vector<face_item_info> face_info;
    face_info.resize(items.size() - 1);
    std::transform(std::next(items.begin()), items.end(), face_info.begin(),
		   parse_face_item);

    bool valid = validate_face_items(face_info);
    assert(valid);

    int t1, t2, t3;
    int n1, n2, n3;
    bool texture = false;
    bool normal = false;

    auto it = face_info.begin();

    texture = ((*it).find(face_item_type::texture) != (*it).end());
    normal = ((*it).find(face_item_type::normal) != (*it).end());

    if (texture)
      t1 = (*it)[face_item_type::texture];
    if (normal)
      n1 = (*it)[face_item_type::normal];
    auto i1 = (*it++)[face_item_type::vertex];
    

    if (texture)
      t2 = (*it)[face_item_type::texture];
    if (normal)
      n2 = (*it)[face_item_type::normal];
    
    auto i2 = (*it++)[face_item_type::vertex];


    
    for(; it != face_info.end(); ++it) {
      // generate triangle for part of face
      auto i3 = (*it)[face_item_type::vertex];
      auto tri = triangle(get_vec(vertices, i1),
			  get_vec(vertices, i2),
			  get_vec(vertices, i3),
			  material);

      // add texture vertices
      if (texture) {
        t3 = (*it)[face_item_type::texture];
	tri.set_vertex_textures(get_vec(textures, t1),
				get_vec(textures, t2),
				get_vec(textures, t3));
      }
      // add vertex normals
      if (normal) {
        n3 = (*it)[face_item_type::normal];
	tri.set_vertex_normals(get_vec(normals, n1),
			       get_vec(normals, n2),
			       get_vec(normals, n3));
      }
     

      // add triangle back to scene and setup lights if relevant.
      sc.add_triangle(tri);
      if (emissive != nullptr) {
	std::cout << "adding light: " << *emissive << std::endl;
	sc.lightsource().add_light(new trilight(tri, *emissive, 1.f));
      }
      // move vertices around to create all triangles for face
      i2 = i3;
      t2 = t3;
      n2 = n3;
    }
  }

  void read_mtl_file(const std::string& dir, const std::string& filename,
		     scene& sc, std::map<std::string, color>& emissives) {
    std::string filepath = dir + filename;
    std::cout << "read mtl file [" << filepath << "]..." << std::endl;

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
    std::cout << "read mtl file done." << std::endl;
  }


  void read_obj_file(const std::string& dir, const std::string& filename,
		     scene& sc, bool allow_lights) {
    std::string filepath = dir + filename;
    std::cout << "read obj file [" << filepath << "]..." << std::endl;
    
    std::ifstream stream(filepath, std::ifstream::in);

    std::vector<vec3f> vertices;
    std::vector<vec3f> textures;
    std::vector<vec3f> normals;
    imaterial* material = nullptr;
    std::map<std::string, color> emissives;
    color *emissive = nullptr;
    
    for (std::string line; std::getline(stream, line);) {
      
      std::vector<std::string> items;
      boost::trim(line);
      boost::split(items, line, boost::is_any_of(" \t"),
		   boost::algorithm::token_compress_on);

      if (items.empty()) {
	continue;
      }

      if (items[0] == "v") {
	read_vertex(vertices, items);
      }
      else if (items[0] == "vt") {
	read_vertex_texture(textures, items);
      }
      else if (items[0] == "vn") {
	read_vertex_normal(normals, items);
      }
      else if (items[0] == "f") {
	read_face(sc, vertices, textures, normals, items, material, emissive);
      }
      else if (items[0] == "mtllib") {
	boost::trim(items[1]);
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
    std::cout << "read obj file done." << std::endl;
  }
}
