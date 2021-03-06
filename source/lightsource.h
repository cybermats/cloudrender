#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include "logger.h"
#include "ilight.h"

class lightsource_t
{
private:
    std::vector<std::unique_ptr<ilight>> _lights;
    size_t _index;
    std::mutex _mtx;

public:
    lightsource_t()
    : _index(0)
    {}

    ray_t generate_ray() {
      assert(!_lights.empty());
      std::lock_guard<std::mutex> lg(_mtx);
        auto ray = _lights[_index++]->generate_ray();
        if (_index >= _lights.size())
            _index = 0;
        return ray;
    }

    void add_light(ilight* light) {
      LOG_DEBUG << "lightsource::add_light()";
      _lights.push_back(std::unique_ptr<ilight>(light));
    }

    bool empty() const {
      return _lights.empty();
    }
    
};
