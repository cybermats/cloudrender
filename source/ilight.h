#pragma once

#include "vecmath.h"
#include "ray.h"

class ilight
{
public:
    virtual ray_t generate_ray() = 0;
    virtual float relevance() const = 0;
};
