#pragma once

#include <vector>
#include <memory>

#include "ilight.h"

class lightsource
{
private:
    std::vector<std::unique_ptr<ilight>> _lights;
    size_t _index;

public:
    lightsource()
    : _index(0)
    {}

    ray generate_ray() {
        auto ray = _lights[_index++]->generate_ray();
        if (_index >= _lights.size())
            _index = 0;
        return ray;
    }

    void add_light(ilight* light) {
        _lights.push_back(std::unique_ptr<ilight>(light));
    }
    
};