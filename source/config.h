#pragma once


namespace config 
{
    const static float ERR = 1e-5;

    struct config
    {
      int max_ray_age;
    };

    config& get_config()
    {
      static config cfg;
      return cfg;
    }
}
