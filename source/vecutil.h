#pragma once

#include <iostream>
#include <typeinfo>
#include "vecmath.h"
#include "color.h"

template<class T>
std::ostream& operator<<(std::ostream& stream, const vec2<T>& vec) {
    stream << "vec2<" << typeid(T).name() << ">(" 
        << vec.x << "," << vec.y << ")";
    return stream;
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const vec3<T>& vec) {
    stream << "vec3<" << typeid(T).name() << ">(" 
        << vec.x << "," << vec.y << "," << vec.z << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const color& c);
std::ostream& operator<<(std::ostream& stream, const matrix3f& m);

matrix3f find_rotation_matrix(const vec3f& a, const vec3f& b);
