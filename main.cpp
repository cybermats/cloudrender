#include <iostream>

#include "vecmath.h"
#include "vecutil.h"
#include "pointlight.h"
#include "lightsource.h"
#include "scene.h"
#include "triangle.h"
#include "imaterial.h"
#include "sampler.h"
#include "render.h"
#include "matte_material.h"

int main() {
    lightsource ls;
    ls.add_light(new pointlight(vec3f(0., 0.9, 0.), color(1, 1, 1), 1));

    scene sc;
    auto white_material = new matte_material(color(1,1,1));

    sc.add_triangle(triangle(vec3f(-1, -1, -1), vec3f(1, -1, -1), vec3f(-1, 1, -1), white_material));
    sc.add_triangle(triangle(vec3f(1, -1, -1), vec3f(-1, 1, -1), vec3f(1, 1, -1), white_material));

    sampler sa(10);

    render r(&ls, &sc, &sa);
    r.run();
    
}

