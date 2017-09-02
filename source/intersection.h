#pragma once

class triangle;

struct intersection
{
    float t;
    triangle* tri;
    vec3f normal;
    ray* ray;
    vec3f position;
};