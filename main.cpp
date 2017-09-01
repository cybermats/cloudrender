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
#include "camera.h"
#include "imagebuffer.h"

void create_white_box(scene &sc, imaterial* material)
{    
    // - z
    sc.add_triangle(triangle(vec3f(-1, -1, -10), vec3f(1, -1, -10), vec3f(-1, 1, -10), material));
    sc.add_triangle(triangle(vec3f(1, -1, -10), vec3f(1, 1, -10), vec3f(-1, 1, -10), material));

    // + z
    sc.add_triangle(triangle(vec3f(-1, -1, 10), vec3f(-1, 1, 10), vec3f(1, -1, 10), material));
    sc.add_triangle(triangle(vec3f(1, -1, 10), vec3f(-1, 1, 10), vec3f(1, 1, 10), material));
    
    // - y
    sc.add_triangle(triangle(vec3f(-1, -1, -10), vec3f(-1, -1, 10), vec3f(1, -1, -10), material));
    sc.add_triangle(triangle(vec3f(1, -1, -10), vec3f(-1, -1, 10), vec3f(1, -1, 10), material));

    // + y
    sc.add_triangle(triangle(vec3f(-1, 1, -10), vec3f(1, 1, -10), vec3f(-1, 1, 10), material));
    sc.add_triangle(triangle(vec3f(1, 1, -10), vec3f(1, 1, 10), vec3f(-1, 1, 10), material));

    // - x
    sc.add_triangle(triangle(vec3f(-1, -1, -10), vec3f(-1, 1, -10), vec3f(-1, -1, 10), material));
    sc.add_triangle(triangle(vec3f(-1, 1, -10), vec3f(-1, 1, 10), vec3f(-1, -1, 10), material));

    // + x
    sc.add_triangle(triangle(vec3f(1, -1, -10), vec3f(1, -1, 10), vec3f(1, 1, -10), material));
    sc.add_triangle(triangle(vec3f(1, 1, -10), vec3f(1, -1, 10), vec3f(1, 1, 10), material));
}

int main() {
    
    lightsource ls;
    ls.add_light(new pointlight(vec3f(0., 0.9, 0.), color(1, 1, 1), 1));

    scene sc;
    matte_material white_material(color(1,1,1));
    
    create_white_box(sc, &white_material);

    vec3f cam_pos(0.0, 0.0, 0.0);
    vec3f cam_up(0, 1, 0);
    vec3f cam_lookat(0, 0, 1);
    float cam_f = 0.035;
    float cam_s2 = 0.036;

    imagebuffer ib(128, 128);

    camera cam(cam_pos, cam_up, cam_lookat, cam_f, cam_s2, &ib);
    // camera
    cam.setup_scene(sc);

    sampler sa(10000000);

    render r(&ls, &sc, &sa);
    r.run();
    std::cout << "Counter: " << cam.counter() << std::endl;
    ib.save("foo.ppm");
    
}

