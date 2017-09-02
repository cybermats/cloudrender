#pragma once

#include <vector>

#include "triangle.h"
#include "ray.h"
#include "vecmath.h"
#include "intersection.h"

class scene
{
public:
    intersection intersect(ray& r)
    {
        auto hit = (triangle*)nullptr;
        auto t_max = std::numeric_limits<float>::max();
        for(auto& tri: _triangles)
        {
            auto t = tri.intersect(r);
            if (t > 0 && t < t_max) {
                t_max = t;
                hit = &tri;
            }
        }

        auto normal = vec3f();
        auto position = vec3f();
        if (hit) {
            normal = hit->normal();
            position = r.origin() + r.direction() * t_max;
        } else {
            t_max = -1;
        }

        return intersection {
            t_max,
            hit,
            normal,
            &r,
            position
        };

    }

    void add_triangle(const triangle& t) {
        _triangles.push_back(t);
    }

private:
    std::vector<triangle> _triangles;
};