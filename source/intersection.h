#pragma once

class triangle_t;

struct intersection_t
{
    float t;
    triangle_t* tri;
    vec3f normal;
    ray_t* ray;
    vec3f position;
};
