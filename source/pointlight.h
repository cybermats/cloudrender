#pragma once

#include <random>

#define _USE_MATH_DEFINES
#include <cmath>

#include "ilight.h"

class pointlight : public ilight
{
private:
    vec3f _origin;
    color _color;
    float _relevance;

    std::mt19937 _gen;
    std::uniform_real_distribution<> _theta_dis;
    std::uniform_real_distribution<> _u_dis;
    
public:
    pointlight(const vec3f& origin, const color& color, float relevance)
    : _origin(origin), _color(color)
    , _relevance(relevance)
    , _gen(std::random_device()())
    , _theta_dis(0, 2 * M_PI)
    , _u_dis(-1, 1)
    {}

    virtual ray generate_ray() override 
    {
        auto theta = _theta_dis(_gen);
        auto u = _u_dis(_gen);
        auto dir = vec3f(
            sqrt(1 - u*u) * cos(theta),
            sqrt(1 - u*u) * sin(theta),
            u
        );
        return ray(_origin, dir);
    }

    virtual float relevance() const override {
        return _relevance;
    }
    
};