#pragma once

#include <iostream>
#include <algorithm>

#include "ray.h"
#include "intersection.h"
#include "sampler.h"
#include "scene.h"
#include "imagebuffer.h"


class camera : public imaterial
{
private:
    int _counter;
    float _f;
    float _s1;
    float _s2;
    vec3f _position;
    vec3f _view_dir;
    vec3f _u;
    vec3f _v;
    imagebuffer *_ib;

public:
    camera(vec3f position, vec3f up, vec3f lookat, float f, float s2, imagebuffer* ib)
    : _counter(0)
    , _f(f), _s2(s2)
    , _ib(ib)
    {
        _position = position;
        _view_dir = (lookat - position).normalize();
        _u = _view_dir.cross(up).normalize();
        _v = _u.cross(_view_dir).normalize();
        
        _s1 = (_f * _s2) / ( _s2 - _f);
        std::cout << "cam pos: " << _position << std::endl;
        std::cout << "cam viewDir: " << _view_dir << std::endl;
        std::cout << "cam v: " << _v << std::endl;
        std::cout << "cam u: " << _u << std::endl;
        std::cout << "cam f: " << _f << std::endl;
        std::cout << "cam s1: " << _s1 << std::endl;
        std::cout << "cam s2: " << _s2 << std::endl;
    }

    void setup_scene(scene& s) {
        auto v0 = _position - (0.1f * _v) - (0.1f * _u);
        auto v1 = _position - (0.1f * _v) + (0.1f * _u);
        auto v2 = _position + (0.1f * _v) - (0.1f * _u);
        auto v3 = _position + (0.1f * _v) + (0.1f * _u);

        auto t1 = triangle(v0, v2, v1, this);
        auto t2 = triangle(v1, v2, v3, this);

        std::cout << "t1.normal: " << t1.normal() << std::endl;
        std::cout << "t2.normal: " << t2.normal() << std::endl;
        
        s.add_triangle(t1);
        s.add_triangle(t2);
    }

    virtual ray shade(const intersection& i) 
    {
        auto neg_dir = -i.ray->direction();
        if (neg_dir.dot(_view_dir) < 0)
            return ray();

        auto v_cos = _v.dot(neg_dir);
        auto v_sin = _v.cross(neg_dir).length();
        auto v_tan = v_sin / v_cos;

        auto u_cos = _u.dot(neg_dir);
        auto u_sin = _u.cross(neg_dir).length();
        auto u_tan = u_sin / u_cos;

        auto pos = i.position - _position;

        auto v_h = _v.dot(pos);
        auto u_h = _u.dot(pos);

//        std::cout << "v_h: " << v_h << std::endl;
//        std::cout << "u_h: " << u_h << std::endl;

        auto v = std::min(std::max(v_h, -0.1f), 0.1f);
        auto u = std::min(std::max(u_h, -0.1f), 0.1f);
        auto x = ((v + 0.1) / 0.2) * _ib->width();
        auto y = ((u + 0.1) / 0.2) * _ib->height();
        
        auto c = _ib->get(x, y);
        c = c + color(.1, .1, .1);
        _ib->set(x, y, c);
        
        
        ++_counter;
        return ray();
    }

    int counter() {
        return _counter;
    }

};