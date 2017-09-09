#pragma once

#include <vector>
#include <cmath>

#include "vecmath.h"
#include "triangle.h"

class kd_tree
{
 private:
  struct bbox
  {
  private:

    static vec3f max(const vec3f& a, const vec3f& b)
    {
      return vec3f(std::max(a.x, b.x),
		   std::max(a.y, b.y),
		   std::max(a.z, b.z));
    }
    
    static vec3f min(const vec3f& a, const vec3f& b)
    {
      return vec3f(std::min(a.x, b.x),
		   std::min(a.y, b.y),
		   std::min(a.z, b.z));
    }
    
  public:
    vec3f bounds[2];
    bbox() = default;
    bbox(const vec3f& vmin, const vec3f& vmax)
    {
      bounds[0] = vmin;
      bounds[1] = vmax;
    }

    bbox(const triangle* tri) {
      bounds[0] = min(min(tri->v0(), tri->v1()), tri->v2());
      bounds[1] = max(max(tri->v0(), tri->v1()), tri->v2());
    }

    void expand(const triangle* tri) {
      auto tri_box = bbox(tri);
      bounds[0] = min(bounds[0], tri_box.bounds[0]);
      bounds[1] = max(bounds[1], tri_box.bounds[1]);
    }

    int longest_axis() const {
      auto tmp = bounds[1] - bounds[0];
      int axis = -1;
      float len = -1;
      for(int i = 0; i < 3; ++i) {
	if (tmp[i] < len)
	  continue;
	axis = i;
	len = tmp[i];
      }
      return axis;
    }

    bool intersect(const ray &r) const
    {
      const auto& inv_dir = r.inv_direction();
      const auto* sign = r.sign();
      const auto& origin = r.origin();

      
      auto tmin = (bounds[sign[0]].x - origin.x) * inv_dir.x;
      auto tmax = (bounds[1-sign[0]].x - origin.x) * inv_dir.x;
      auto tymin = (bounds[sign[1]].y - origin.y) * inv_dir.y;
      auto tymax = (bounds[1-sign[1]].y - origin.y) * inv_dir.y;

      if ((tmin > tymax) || (tymin > tmax)) {
	return false;
      }
      if (tymin > tmin)
	tmin = tymin;
      if (tymax < tmax)
	tmax = tymax;

      auto tzmin = (bounds[sign[2]].z - origin.z) * inv_dir.z;
      auto tzmax = (bounds[1-sign[2]].z - origin.z) * inv_dir.z;

      
      if ((tmin > tzmax) || (tzmin > tmax)) {
	return false;
      }
      return true;
    }
  };

  class kd_node {
  private:
    bbox _bbox;
    kd_node* left = nullptr;
    kd_node* right = nullptr;
    std::vector<triangle*> _triangles;
    int _depth;

  public:
    ~kd_node() {
      if (left)
	delete left;
      if (right)
	delete right;
    }
    
  kd_node(const std::vector<triangle*> tris, int depth)
    : _bbox()
      , left(nullptr)
      , right(nullptr)
      , _triangles(tris)
      , _depth(depth) {
      if (_triangles.empty())
	return;

      _bbox = bbox(_triangles[0]);
	
      if (_triangles.size() == 1) {
	return;
      }

      vec3f mid = _triangles[0]->midpoint();

      for(int i = 1; i < _triangles.size(); ++i) {
	_bbox.expand(_triangles[i]);
	mid = mid + _triangles[i]->midpoint();
      }

      if (_triangles.size() < 10)
	return;
      
      mid = mid  / _triangles.size();

      std::vector<triangle*> left_tris;
      std::vector<triangle*> right_tris;
      int axis = _bbox.longest_axis();
      for(auto tri : _triangles) {
	const auto i_mid = tri->midpoint();
	switch (axis) {
	case 0:
	  if (mid.x < i_mid.x)
	    left_tris.push_back(tri);
	  else
	    right_tris.push_back(tri);
	  break;
	case 1:
	  if (mid.y < i_mid.y)
	    left_tris.push_back(tri);
	  else
	    right_tris.push_back(tri);
	  break;
	case 2:
	  if (mid.z < i_mid.z)
	    left_tris.push_back(tri);
	  else
	    right_tris.push_back(tri);
	  break;
	}
      }

      if (left_tris.empty() || right_tris.empty())
	return;

      left = new kd_node(left_tris, _depth + 1);
      right = new kd_node(right_tris, _depth + 1);
    }

    triangle* intersect(const ray& r, float &t, float &u, float &v) {
      if (!_bbox.intersect(r))
	return nullptr;
      triangle* hit_left = nullptr;
      triangle* hit_right = nullptr;
      float tl;
      float tr;
      if (left)
	hit_left = left->intersect(r, tl, u, v);
      if (right)
	hit_right = right->intersect(r, tr, u, v);
      if (hit_left && hit_right) {
	if (tl < tr) {
	  t = tl;
	  return hit_left;
	}
	else {
	  t = tr;
	  return hit_right;
	}
      }
      else if (hit_left) {
	t = tl;
	return hit_left;
      }
      else if (hit_right) {
	t = tr;
	return hit_right;
      }

      auto hit = (triangle*)nullptr;
      t = std::numeric_limits<float>::max();
      for(auto tri: _triangles) {
	auto t_t = tri->intersect(r, u, v);
	if (t_t > 0 && t_t < t) {
	  t = t_t;
	  hit = tri;
	}
      }
      
      return hit;
    }

    void print() {
      std::cout << "kd_node(" << _depth << ") min: " << _bbox.bounds[0]
		<< ", max: " << _bbox.bounds[1]
	        << ", nodes: " << _triangles.size()
	        << ", left: " << (left != nullptr) << ", right: " << (right != nullptr)
		<< std::endl;

      if (left)
	left->print();
      if (right)
	right->print();
    }
  };

  std::vector<triangle*> _triangles;
  bool _initialized;
  kd_node* _root;
  
 public:

  kd_tree()
  : _initialized(false)
  , _root(nullptr)
  {}
  
  ~kd_tree() {
    if (_root)
      delete _root;
    for(auto tri : _triangles)
      delete tri;
  }

  void add_triangle(triangle* tri) {
    assert(!_initialized);
    _triangles.push_back(tri);
  }

  void initialize() {
    assert(!_initialized);
    _initialized = true;
    _root = new kd_node(_triangles, 0);
    _root->print();
  }

  triangle* intersect(const ray& r, float &t, float &u, float &v) {
    assert(_initialized);
    if (_root)
      return _root->intersect(r, t, u, v);
    return nullptr;
  }
  
    
};
