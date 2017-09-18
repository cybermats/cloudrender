#pragma once

#include <cstdlib>

#include "vecmath.h"
#include "vecutil.h"
#include "ray.h"
#include "config.h"
#include "imaterial.h"

class triangle_t
{
private:
    vec3f _v0;
    vec3f _v1;
    vec3f _v2;

    vec3f _nv0;
    vec3f _nv1;
    vec3f _nv2;

    vec3f _tv0;
    vec3f _tv1;
    vec3f _tv2;

    vec3f _normal;

    imaterial* _material;

    friend std::ostream& operator<<(std::ostream& stream, const triangle_t& tri);

public:
    triangle_t(const vec3f& v0, const vec3f& v1, const vec3f& v2, imaterial* material)
    : _v0(v0), _v1(v1), _v2(v2), _material(material)
    {
        _normal = (_v1 - _v0).cross(_v2 - _v0);
        _normal = _normal * ( 1 / _normal.length());
	_nv0 = _nv1 = _nv2 = _normal;
    }

    void set_vertex_normals(const vec3f& nv0, const vec3f& nv1, const vec3f& nv2)
    {
      _nv0 = nv0;
      _nv1 = nv1;
      _nv2 = nv2;
    }

    void set_vertex_textures(const vec3f& tv0, const vec3f& tv1, const vec3f& tv2)
    {
      _tv0 = tv0;
      _tv1 = tv1;
      _tv2 = tv2;
    }
    

    const vec3f& normal() const 
    {
        return _normal;
    }

    const vec3f normal(float u, float v) const 
    {
      return (((1.f - u - v) * _nv0) + (u * _nv1) + (v * _nv2)).normalize();
    }

    imaterial* material() const
    {
        return _material;
    }

    float intersect(const ray_t& r, float& u, float& v) 
    {
        auto e1 = _v1 - _v0;
        auto e2 = _v2 - _v0;

        auto pvec = r.direction().cross(e2);
        auto det = e1.dot(pvec);

        if (det < config::ERR && det > -config::ERR) {
            return 0;
        }

        auto inv_det = 1 / det;
        auto tvec = r.origin() - _v0;
        u = tvec.dot(pvec) * inv_det;
        if (u < 0 || u > 1) {
            return 0;
        }

        auto qvec = tvec.cross(e1);
        v = r.direction().dot(qvec) * inv_det;
        if (v < 0 || u + v > 1) {
            return 0;
        }

        return e2.dot(qvec) * inv_det;
    }

    const vec3f& v0() const {
      return _v0;
    }
    const vec3f& v1() const {
      return _v1;
    }
    const vec3f& v2() const {
      return _v2;
    }

    vec3f midpoint() const {
      return (_v0 + _v1 + _v2) * (1.f / 3.f);
    }

};

std::ostream& operator<<(std::ostream& stream, const triangle_t& tri);
