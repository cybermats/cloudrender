#pragma once

#include "vecmath.h"
#include <iostream>
#include <typeinfo>

template<class T>
std::ostream& operator<<(std::ostream& stream, const vec2<T> vec) {
    stream << "vec2<" << typeid(T).name() << ">(" 
        << vec.x << "," << vec.y << ")";
    return stream;
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const vec3<T> vec) {
    stream << "vec3<" << typeid(T).name() << ">(" 
        << vec.x << "," << vec.y << "," << vec.z << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const color c) {
    stream << "color(" 
        << c.r << "," << c.g << "," << c.b << "," << c.a << ")";
    return stream;
}