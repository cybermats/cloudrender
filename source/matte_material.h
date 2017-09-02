#pragma once

#include <random>

#define _USE_MATH_DEFINES
#include <cmath>

#include "imaterial.h"
#include "ray.h"
#include "intersection.h"
#include "vecmath.h"
#include "config.h"

class matte_material : public imaterial
{
private:
    color _color;

    std::mt19937 _gen;
    std::uniform_real_distribution<> _theta_dis;
    std::uniform_real_distribution<> _phi_dis;

public:
    matte_material(const color& color)
    : _color(color)
    , _gen(std::random_device()())
    , _theta_dis(0, 2 * M_PI)
    , _phi_dis(0, M_PI / 2)
    {}

    virtual ray shade(const intersection& i) override {
        auto unit = vec3f(0, 0, 1);

        auto rot_mat = find_rotation_matrix(unit, i.normal);

        
        // generate new vector
        auto theta = _theta_dis(_gen);
        auto phi = _phi_dis(_gen);

        auto dir =  vec3f(
            cos(theta) * sin(phi),
            sin(theta) * sin(phi),
            cos(phi)
        );
        dir = rot_mat * dir;
        assert(dir.length() - 1 < config::ERR);
        return ray(i.position + dir * config::ERR, dir, i.ray->age()+1);
    }
    
};