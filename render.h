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
        while(!_sampler->done()) {
            auto ray = _lights->generate_ray();
            while(ray.active())
            {
                auto inter = _scene->intersect(ray);
                auto material = inter.tri->material();
                ray = material->shade(inter);
            }
        }
    }
};