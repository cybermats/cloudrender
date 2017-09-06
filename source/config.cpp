#include "config.h"

namespace config
{
  config& get_config()
  {
    static config cfg;
    return cfg;
  }

}
