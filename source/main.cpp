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
#include "obj_file.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

void create_white_box(scene &sc)
{
  float length =1;
  float height = 1;
  float width = 1;
  imaterial* white = sc.material().lookup_material("white");
  imaterial* red = sc.material().lookup_material("red");
  imaterial* green = sc.material().lookup_material("green");
  imaterial* blue = sc.material().lookup_material("blue");
  
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

void create_camera_static(scene& sc, radiance_buffer& rb)
{
  vec3f cam_pos(1, 1, 0.0);
  vec3f cam_up(0, 1, 0);
  vec3f cam_lookat(0, 0, 0);
  float cam_object_dist = 1;
  float cam_image_dist = .035;

  sc.add_camera(cam_pos, cam_up, cam_lookat, cam_object_dist, cam_image_dist, &rb);
}

void create_camera_load(scene& sc, radiance_buffer& rb)
{
  vec3f cam_pos(0, 1, 1);
  vec3f cam_up(0, 1, 0);
  vec3f cam_lookat(0, 1, 0);
  float cam_object_dist = 1;
  float cam_image_dist = .035;

  sc.add_camera(cam_pos, cam_up, cam_lookat, cam_object_dist, cam_image_dist, &rb);
}

void load_scene_static(scene& sc, radiance_buffer& rb)
{
  sc.lightsource().add_light(new pointlight(vec3f(0.0, 0.9, 0.), color(1, 1, 1, 1.0), 1));
  sc.material().add_material("white", new matte_material(color(.9,.9,.9, 1.0)));
  sc.material().add_material("red",   new matte_material(color(.9,.5,.5, 1.0)));
  sc.material().add_material("green", new matte_material(color(.5,.9,.5, 1.0)));
  sc.material().add_material("blue",  new matte_material(color(.5,.5,.9, 1.0)));
  
  create_white_box(sc);
  create_camera_static(sc, rb);
}

void load_scene_obj(scene& sc, radiance_buffer& rb)
{
  sc.lightsource().add_light(new pointlight(vec3f(0, 1.7, 0), color(1, 1, 1, 1.0), 1));
  auto mat = sc.material().add_material("white",
					new matte_material(color(.9,.9,.9, 1.0)));
  std::string filename = "../data/CornellBox-Original.obj";
  obj_reader::read_obj_file(filename, sc);

  create_camera_load(sc, rb);
}

void render_pass(scene& sc, int samples, bool show_progress, int threads)
{
  sampler sa(samples, show_progress);
  render r(&sc, &sa);
  r.run(threads);
}

void image_pass(radiance_buffer& rb, imagebuffer& ib, int min_age, int max_age)
{
  for(auto& s : rb) {
    int x = ((s.plate_pos.x + 0.1) / 0.2) * ib.width();
    int y = ((s.plate_pos.y + 0.1) / 0.2) * ib.height();

    x = std::max(std::min(x, (int)ib.width()-1), 0);
    y = std::max(std::min(y, (int)ib.height()-1), 0);

    if (s.ray_age >= min_age && s.ray_age <= max_age) {
      ib.inc(x, y, s.color);
    }
  }
}

int main(int argc, char** argv) {
  po::options_description desc("Allowed options");
  int samples;
  std::string output_filename;
  std::string input_filename;
  bool only_radiance;
  bool input_present = false;
  int image_width, image_height;
  double image_gamma;
  bool continuous;
  int max_bounce_rate;
  int threads;
  int min_age;
  int max_age;

  try {

    desc.add_options()
      ("help", "produce help message")
	 
      ("samples", po::value<int>(&samples)->default_value(100000),
       "number of rays created")
	 
      ("input,I", po::value<std::string>(&input_filename),
       "input radiance file")
	 
      ("output,O", po::value<std::string>(&output_filename)->required(),
       "output image file")

      ("only-radiance,R", po::value<bool>(&only_radiance),
       "only output radiance, not an image")

      ("image-width,W", po::value<int>(&image_width)->default_value(128),
       "width of the output image file")
	 
      ("image-height,H", po::value<int>(&image_height)->default_value(128),
       "height of the output image file")
	 
      ("image-gamma,G", po::value<double>(&image_gamma)->default_value(2.2),
       "gamma of the output image file")

      ("continuous,C", po::value<bool>(&continuous)->default_value(false),
       "if true then this will run continuously")

      ("max-bouce-rate", po::value<int>(&max_bounce_rate)->default_value(4),
       "max bounces of rays")

      ("threads", po::value<int>(&threads)->default_value(4),
       "number of threads to run on in parallel")

      ("min-age", po::value<int>(&min_age)->default_value(1),
       "minimum age of rays to be included in image")
	 
      ("max-age", po::value<int>(&max_age)->default_value(10),
       "maximum age of rays to be included in image")
	 
      ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 1;
    }

    po::notify(vm);

    if (continuous && !only_radiance) {
      std::cout << "have to specify only-radiance if running continuously"
		<< std::endl;
      return 1;
    }

    if (vm.count("input")) {
      input_present = true;
    }

  }
  catch(po::required_option& ro) {
    std::cerr << ro.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return 1;
  }
  catch(std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  catch(...) {
    std::cerr << "Unknown error!" << std::endl;
    return 1;
  }

  //  try {

    config::get_config().max_ray_age = max_bounce_rate;

    if (continuous) {
      scene sc;
      radiance_buffer rb;
      load_scene_static(sc, rb);
      while(true) {
	render_pass(sc, 1000000, false, threads); 
	std::ofstream rf(output_filename, std::ofstream::out | std::ofstream::app);
	rf << rb;
	std::cout << ".";
	std::cout.flush();
	rb.clear();
      }
    }

    radiance_buffer rb;
    if (input_present) {
      std::cout << "Reading radiance file from: " << input_filename << std::endl;
      std::ifstream rf(input_filename, std::ifstream::in);
      rf >> rb;
    } else {
      std::cout << "Rendering..." << std::endl;
      scene sc;
      load_scene_obj(sc, rb);
      //    load_scene_static(sc, rb);
      render_pass(sc, samples, true, threads);
      std::cout << "Samples hit: " << sc.get_camera().counter() << std::endl;
    }

    if (only_radiance) {
      std::cout << "Saving radiance file to: " << output_filename << std::endl;
      std::ofstream rf(output_filename, std::ofstream::out);
      rf << rb;
    } else {
      imagebuffer ib(image_width, image_height);
      image_pass(rb, ib, min_age, max_age);
      std::cout << "Saving image file to: " << output_filename << std::endl;
      ib.save(output_filename.c_str(), image_gamma);
    }
    /*
  }
  catch(std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  catch(...) {
    std::cerr << "Unknown error!" << std::endl;
    return 1;
    }*/
}

