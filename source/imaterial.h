#pragma once

#include "vecmath.h"
#include "ray.h"

class intersection;

class imaterial
{
public:
    virtual ray shade(const intersection& r) = 0;
};