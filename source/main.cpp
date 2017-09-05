#include <iostream>
#include <sstream>

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
#include "radiance_sample.h"

void create_white_box(scene &sc,
		      imaterial* white,
		      imaterial* red,
		      imaterial* green,
		      imaterial* blue)
{
  float length =1;
  float height = 1;
  float width = 1;
  // - z
  sc.add_triangle(triangle(vec3f(-width, -height, -length),
			   vec3f(width, -height, -length),
			   vec3f(-width, height, -length),
			   red));
  
  sc.add_triangle(triangle(vec3f(width, -height, -length),
			   vec3f(width, height, -length),
			   vec3f(-width, height, -length),
			   red));

  // + z
  sc.add_triangle(triangle(vec3f(-width, -height, length),
			   vec3f(-width, height, length),
			   vec3f(width, -height, length),
			   red));
  
  sc.add_triangle(triangle(vec3f(width, -height, length),
			   vec3f(-width, height, length),
			   vec3f(width, height, length),
			   red));
    
  // - y
  sc.add_triangle(triangle(vec3f(-width, -height, -length),
			   vec3f(-width, -height, length),
			   vec3f(width, -height, -length),
			   white));
  
  sc.add_triangle(triangle(vec3f(width, -height, -length),
			   vec3f(-width, -height, length),
			   vec3f(width, -height, length),
			   white));

  // + y
  sc.add_triangle(triangle(vec3f(-width, height, -length),
			   vec3f(width, height, -length),
			   vec3f(-width, height, length),
			   white));
  
  sc.add_triangle(triangle(vec3f(width, height, -length),
			   vec3f(width, height, length),
			   vec3f(-width, height, length),
			   white));

  // - x
  sc.add_triangle(triangle(vec3f(-width, -height, -length),
			   vec3f(-width, height, -length),
			   vec3f(-width, -height, length),
			   white));
  
  sc.add_triangle(triangle(vec3f(-width, height, -length),
			   vec3f(-width, height, length),
			   vec3f(-width, -height, length),
			   white));

  // + x
  sc.add_triangle(triangle(vec3f(width, -height, -length),
			   vec3f(width, -height, length),
			   vec3f(width, height, -length),
			   white));
  
  sc.add_triangle(triangle(vec3f(width, height, -length),
			   vec3f(width, -height, length),
			   vec3f(width, height, length),
			   white));
}

void render_pass(radiance_buffer& rb)
{
  
  std::string filename_base = "image";
  std::string filename_ext = ".ppm";
    
    lightsource ls;
    ls.add_light(new pointlight(vec3f(0.0, 0.9, 0.), color(1, 1, 1, 1.0), 1));

    scene sc;
    matte_material white_material(color(.9,.9,.9, 1.0));
    matte_material red_material(color(.9,.5,.5, 1.0));
    matte_material green_material(color(.5,.9,.5, 1.0));
    matte_material blue_material(color(.5,.5,.9, 1.0));
    
    create_white_box(sc, &white_material, &red_material, &green_material, &blue_material);

    vec3f cam_pos(0, 0, 0.0);
    vec3f cam_up(0, 1, 0);
    vec3f cam_lookat(1, -0.0, 1);
    //    float cam_f = 0.035;
    //    float cam_s2 = 0.036;
    float cam_object_dist = 1.41;
    float cam_image_dist = .035;

    //    imagebuffer ib(128, 128);

    camera cam(cam_pos, cam_up, cam_lookat, cam_object_dist, cam_image_dist, &rb);
    // camera
    cam.setup_scene(sc);

    sampler sa(1000000);

    render r(&ls, &sc, &sa);

    r.run(4);
    std::cout << "Counter: " << cam.counter() << std::endl;

    std::string filename = filename_base + filename_ext;
    std::cout << "Saving: " << filename << std::endl;
    //    ib.save(filename.c_str());

}

void image_pass(radiance_buffer& rb, const char* filename)
{
  
  
}

int main() {
  radiance_buffer rb;
  render_pass(rb);
  std::ofstream f("image.txt", std::ofstream::out);
  f << rb;
  f.close();
}

