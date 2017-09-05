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

 #include <boost/program_options.hpp>

 namespace po = boost::program_options;

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

void render_pass(radiance_buffer& rb, int samples, bool show_progress, int threads)
 {
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
   float cam_object_dist = 1;
   float cam_image_dist = .035;

   camera cam(cam_pos, cam_up, cam_lookat, cam_object_dist, cam_image_dist, &rb);

   // camera
   cam.setup_scene(sc);

   sampler sa(samples, show_progress);

   render r(&ls, &sc, &sa);

   r.run(threads);
 }

 void image_pass(radiance_buffer& rb, imagebuffer& ib)
 {
   for(auto& s : rb) {
     int x = ((s.plate_pos.x + 0.1) / 0.2) * ib.width();
     int y = ((s.plate_pos.y + 0.1) / 0.2) * ib.height();

     x = std::max(std::min(x, (int)ib.width()-1), 0);
     y = std::max(std::min(y, (int)ib.height()-1), 0);

     ib.inc(x, y, s.color);
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
   int max_ray_age;
   int threads;

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

	 ("max-ray-age", po::value<int>(&max_ray_age)->default_value(4),
	  "max age of rays")

	 ("threads", po::value<int>(&threads)->default_value(4),
	  "number of threads to run on in parallel")
	 
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

   config::get_config().max_ray_age = max_ray_age;

   if (continuous) {
     while(true) {
       radiance_buffer rb;
       render_pass(rb, 1000000, false, threads); 
       std::ofstream rf(output_filename, std::ofstream::out | std::ofstream::app);
       rf << rb;
       std::cout << ".";
       std::cout.flush();
     }
   }

   radiance_buffer rb;
   if (input_present) {
     std::cout << "Reading radiance file from: " << input_filename << std::endl;
     std::ifstream rf(input_filename, std::ifstream::in);
     rf >> rb;
   } else {
     std::cout << "Rendering..." << std::endl;
     render_pass(rb, samples, true, threads);
   }

   if (only_radiance) {
     std::cout << "Saving radiance file to: " << output_filename << std::endl;
     std::ofstream rf(output_filename, std::ofstream::out);
     rf << rb;
   } else {
     imagebuffer ib(image_width, image_height);
     image_pass(rb, ib);
     std::cout << "Saving image file to: " << output_filename << std::endl;
     ib.save(output_filename.c_str(), image_gamma);
   }
 }

