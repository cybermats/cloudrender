#pragma once

#include <string>


#include "scene.h"




namespace obj_reader
{
  
  void read_obj_file(std::string dir, std::string filename, scene& sc);
};
