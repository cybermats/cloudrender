#pragma once

#include <cstdlib>

#include "vecmath.h"
#include "vecutil.h"
#include "ray.h"
#include "config.h"
#include "imaterial.h"

class triangle 
{
private:
    vec3f _v0;
    vec3f _v1;
    vec3f _v2;

    vec3f _normal;

    imaterial* _material;

    friend std::ostream& operator<<(std::ostream& stream, const triangle& tri);

public:
    triangle(const vec3f& v0, const vec3f& v1, const vec3f& v2, imaterial* material)
    : _v0(v0), _v1(v1), _v2(v2), _material(material)
    {
        _normal = (_v1 - _v0).cross(_v2 - _v0);
        _normal = _normal * ( 1 / _normal.length());
    }

    const vec3f& normal() const 
    {
        return _normal;
    }

    imaterial* material() const
    {
        return _material;
    }

    float intersect(const ray& r) 
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
        auto u = tvec.dot(pvec) * inv_det;
        if (u < 0 || u > 1) {
            return 0;
        }

        auto qvec = tvec.cross(e1);
        auto v = r.direction().dot(qvec) * inv_det;
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

};

std::ostream& operator<<(std::ostream& stream, const triangle& tri);
