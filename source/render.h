#pragma once

#include "lightsource.h"
#include "scene.h"
#include "sampler.h"

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

    void run() {
        std::cout << "Render!" << std::endl;
        while(!_sampler->done()) {
//            std::cout << "Generating ray..." << std::endl;
            auto ray = _lights->generate_ray();
            while(ray.active())
            {
                auto inter = _scene->intersect(ray);
                if (inter.t >= 0) {
//                    std::cout << "Hit!" << std::endl;
                    auto material = inter.tri->material();
                    ray = material->shade(inter);
                } else {
//                    std::cout << "Miss!" << std::endl;
                    break;
                }
            }
        }
    }
};