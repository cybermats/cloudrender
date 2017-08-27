#pragma once

#include <cassert>
#include <cstdlib>

#include "vecmath.h"
#include "config.h"

class ray
{
private:
    vec3f _origin;
    vec3f _direction;
    int _age;

public:
    ray(vec3f origin, vec3f direction, int age = 0) 
    : _origin(origin)
    , _direction(direction)
    , _age(age)
    {
        assert(std::abs(_direction.length() - 1.) < config::ERR);
    }

    const vec3f& origin() const {
        return _origin;
    }

    const vec3f& direction() const {
        return _direction;
    }

    bool active() const {
        return _age < config::MAX_RAY_AGE;
    }



};

std::ostream& operator<<(std::ostream& stream, const ray& r) {
    stream << "ray( origin: " << r.origin() << ", dir: " << r.direction() << ")";
    return stream;
}
