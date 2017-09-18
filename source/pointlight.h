#pragma once

#include <random>

#define _USE_MATH_DEFINES
#include <cmath>

#include "ilight.h"
#include "color.h"

class pointlight : public ilight
{
private:
    vec3f _origin;
    color_t _color;
    float _relevance;

    std::mt19937 _gen;
    std::uniform_real_distribution<> _theta_dis;
    std::uniform_real_distribution<> _u_dis;
    
public:
    pointlight(const vec3f& origin, const color_t& color, float relevance)
    : _origin(origin), _color(color)
    , _relevance(relevance)
    , _gen(std::random_device()())
    , _theta_dis(0, 2 * M_PI)
      , _u_dis(-1, std::nextafter(1, 2))
    {}

    virtual ray_t generate_ray() override 
    {
        auto theta = _theta_dis(_gen);
        auto u = _u_dis(_gen);
	auto r = sqrt(1 - u*u);
	
        auto dir = vec3f(
            r * cos(theta),
            r * sin(theta),
            u
        );
	
        return ray_t(_origin, dir, _color);
    }

    virtual float relevance() const override {
        return _relevance;
    }
    
};
