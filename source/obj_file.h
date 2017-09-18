#pragma once

#include <string>


#include "scene.h"




namespace obj_reader
{
  
  void read_obj_file(const std::string& dir, const std::string& filename,
		     scene_t& sc, bool allow_lights);
};
