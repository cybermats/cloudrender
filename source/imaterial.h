#pragma once

#include "vecmath.h"
#include "ray.h"

struct intersection_t;

class imaterial
{
public:
    virtual ray_t shade(const intersection_t& r) = 0;
};
