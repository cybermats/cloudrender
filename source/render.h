#pragma once

#include "lightsource.h"
#include "scene.h"
#include "sampler.h"

#include <future>

class render
{
private:
    lightsource* _lights;
    scene* _scene;
    sampler* _sampler;

public:
    render(lightsource* lights, scene* scene, sampler* sampler)
    : _lights(lights)
    , _scene(scene)
    , _sampler(sampler)
    {}

    void run(int thread_count) {
      std::vector<std::future<void> > futures;
      for(int i = 0; i < thread_count; ++i) {
	auto func = [this](){this->run();};
	futures.push_back(std::async(std::launch::async, func));
      }
      for(auto& f : futures) {
	f.wait();
      }
    }

    void run() {
        while(!_sampler->done()) {
            auto ray = _lights->generate_ray();
            while(ray.active())
            {
                auto inter = _scene->intersect(ray);
                if (inter.t >= 0) {
                    auto material = inter.tri->material();
                    ray = material->shade(inter);
                } else {
                    break;
                }
            }
        }
    }
};
