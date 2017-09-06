#pragma once

#include <map>

#include "imaterial.h"

class material_collection
{
 private:
  std::map<std::string, std::unique_ptr<imaterial>> _map;
 public:
  imaterial* add_material(const std::string& name, imaterial* material)
  {
    _map[name] = std::unique_ptr<imaterial>(material);
    return material;
  }

  imaterial* lookup_material(const std::string& name)
  {
    return _map.at(name).get();
  }
  
};
