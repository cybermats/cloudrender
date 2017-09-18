#include <iostream>
#include <sstream>
#include <chrono>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

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
#include "logger.h"


// todo: add refractive material
// todo: add reflective material
// todo: add texture mapping
// todo: add config file
// todo: add support for openexr
// todo: add support for live rendering
// todo: add support for circular aperture
// todo: add benchmark object
// todo: look over the accelerator

// done:
// done: add support for obj file name

namespace po = boost::program_options;

void create_camera_load(scene_t& sc, radiance_buffer& rb)
{
  vec3f cam_pos(0, 1, 4);
  vec3f cam_up(0, 1, 0);
  vec3f cam_lookat(0, 1, 0);
  float cam_focal = 0.0350;
  float cam_hfov = 3;
  float cam_fstop = 0.5f;

  sc.add_camera(cam_pos, cam_up, cam_lookat, cam_focal, cam_hfov, cam_fstop, &rb);
}

void load_scene_obj(scene_t& sc, radiance_buffer& rb, const std::string& filepath)
{
  bool allow_lights = true;
  if (!allow_lights)
    sc.lightsource().add_light(new pointlight(vec3f(0, 1.7, 0), color_t(1, 1, 1, 1.0), 1));
  boost::filesystem::path path(filepath);
  std::string filedir = path.parent_path().string();
  std::string filename = path.filename().string();
  obj_reader::read_obj_file(filedir, filename, sc, allow_lights);

  create_camera_load(sc, rb);
}

void render_pass(scene_t& sc, int samples, bool show_progress, int threads)
{
  sampler sa(samples, show_progress);
  render_t r(&sc, &sa);
  r.run(threads);
}

void image_pass_direct(radiance_buffer& rb, imagebuffer_t& ib, int min_age, int max_age)
{
  for(auto& s : rb) {
    int x = ((s.plate_pos.x + 1) / 2.f) * ib.width();
    int y = ((s.plate_pos.y + 1) / 2.f) * ib.height();

    x = std::max(std::min(x, (int)ib.width()-1), 0);
    y = std::max(std::min(y, (int)ib.height()-1), 0);

    if (s.ray_age >= min_age && s.ray_age <= max_age) {
      ib.inc(x, y, s.color);
    }
  }
}

void image_pass_tent(radiance_buffer& rb, imagebuffer_t& ib, int min_age, int max_age)
{
  for(auto& s : rb) {
    auto u = (s.plate_pos.x + 1.f) / 2.f;
    auto v = (s.plate_pos.y + 1.f) / 2.f;

    u = std::max(std::min(u, 1.f), 0.f);
    v = std::max(std::min(v, 1.f), 0.f);

    if (s.ray_age >= min_age && s.ray_age <= max_age) {
      ib.add(u, v, s.color);
    }
  }
}

bool validate_path_is_file(const std::string filepath)
{
  if (filepath.empty())
    return true;
  boost::filesystem::path p(filepath);
  if (boost::filesystem::exists(p)) {
    return boost::filesystem::is_regular_file(p);
  }
  return false;
}

int main(int argc, char** argv) {
  po::options_description desc("Allowed options");
  int samples;
  std::string output_image_filename;
  std::string output_radiance_filename;
  std::string input_radiance_filename;
  std::string input_scene_filename;
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
	 
      ("input-scene,I", po::value<std::string>(&input_scene_filename),
       "input scene file")

      ("input-radiance", po::value<std::string>(&input_radiance_filename),
       "input radiance file")
	 
      ("output-image,O", po::value<std::string>(&output_image_filename),
       "output image file")

      ("output-radiance", po::value<std::string>(&output_radiance_filename),
       "output radiance file")

      ("image-width,W", po::value<int>(&image_width)->default_value(128),
       "width of the output image file")
	 
      ("image-height,H", po::value<int>(&image_height)->default_value(128),
       "height of the output image file")
	 
      ("gamma", po::value<double>(&image_gamma)->default_value(2.2),
       "gamma of the output image file")

      ("continuous,C", po::value<bool>(&continuous)->default_value(false),
       "if true then this will run continuously")
 
      ("max-bounce-rate", po::value<int>(&max_bounce_rate)->default_value(4),
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

    if (input_radiance_filename.empty() && input_scene_filename.empty()) {
      LOG_ERROR << "have to specify one type of input file.";
      return 1;
    }
    if ((!input_radiance_filename.empty()) && (!input_scene_filename.empty())) {
      LOG_ERROR << "cannot specify both a radiance and scene input file.";
      return 1;
    }

    if (output_image_filename.empty() && output_radiance_filename.empty()) {
      LOG_ERROR << "have to specify at least one type of output file.";
      return 1;
    }

    if (continuous && output_radiance_filename.empty()) {
      LOG_ERROR << "have to specify a radiance output file if running continuously.";
      return 1;
    }
    
    if (continuous && input_scene_filename.empty()) {
      LOG_ERROR << "have to specify the scene file if running continuously.";
      return 1;
    }

    if (!validate_path_is_file(input_scene_filename)) {
      LOG_ERROR << "input scene file does not exist or incorrect path.";
      return 1;
    }

    if (!validate_path_is_file(input_radiance_filename)) {
      LOG_ERROR << "input radiance file does not exist or incorrect path.";
      return 1;
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
    scene_t sc;
    radiance_buffer rb;
    load_scene_obj(sc, rb, input_scene_filename);
    sc.initialize();
    LOG_DEBUG << "Camera focal: " << sc.get_camera().f();
    while(true) {
      render_pass(sc, samples, false, threads); 
      std::ofstream rf(output_radiance_filename,
		       std::ofstream::out | std::ofstream::app);
      rf << rb;
      std::cout << ".";
      std::cout.flush();
      rb.clear();
    }
  }

  radiance_buffer rb;
  if (!input_radiance_filename.empty()) {
    LOG_INFO << "Reading radiance file from: " << input_radiance_filename;
    std::ifstream rf(input_radiance_filename, std::ifstream::in);
    rf >> rb;
  }

  if (!input_scene_filename.empty()) {
    LOG_INFO << "Rendering...";
    scene_t sc;
    load_scene_obj(sc, rb, input_scene_filename);
    LOG_DEBUG << "Camera focal: " << sc.get_camera().f();
    LOG_DEBUG << "Camera di: " << sc.get_camera().d_i();
    LOG_DEBUG << "Camera do: " << sc.get_camera().d_o();
    LOG_DEBUG << "Camera h: " << sc.get_camera().h();
    LOG_DEBUG << "Camera aperture: " << sc.get_camera().aperture();
    LOG_INFO << "Initializing scene...";
    sc.initialize();

    LOG_INFO << "Render...";
    auto t1 = std::chrono::high_resolution_clock::now();
      
    render_pass(sc, samples, true, threads);

    auto t2 = std::chrono::high_resolution_clock::now();
    LOG_DEBUG << "Samples hit: " << sc.get_camera().counter();
    auto dur_sec = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    auto dur_mil = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    LOG_INFO << "Time: " << dur_sec << "." << dur_mil;
  }

  if (!output_radiance_filename.empty()) {
    LOG_INFO << "Saving radiance file to: " << output_radiance_filename;
    std::ofstream rf(output_radiance_filename, std::ofstream::out);
    rf << rb;
  }
  if (!output_image_filename.empty()) {
    imagebuffer_t ib(image_width, image_height);
    image_pass_direct(rb, ib, min_age, max_age);
    LOG_INFO << "Saving image file to: " << output_image_filename;
    ib.save(output_image_filename.c_str(), image_gamma);
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

