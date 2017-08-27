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

std::ostream& operator<<(std::ostream& stream, const matrix3f m) {
    stream << "matrix3f( " 
        << "( " << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << "), "
        << "( " << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << "), "
        << "( " << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ") )";
    return stream;
}


matrix3f find_rotation_matrix(const vec3f& a, const vec3f& b)
{
    assert(a.length() - 1 < config::ERR);
    assert(b.length() - 1 < config::ERR);
    auto axis = a.cross(b);
    auto len = axis.length();

    auto rcos = a.dot(b);
    auto rsin = axis.length();

    if (len < config::ERR && len > -config::ERR) {
        auto bp = b;
        if (bp.x < 0.9)
            bp.x = 1;
        else if (bp.y < 0.9)
            bp.y = 1;
        else 
            bp.z = 1;
        axis = a.cross(bp);
        len = axis.length();
    }
    
    auto inv_len = 1 / len;

    auto u = axis.x * inv_len;
    auto v = axis.y * inv_len;
    auto w = axis.z * inv_len;
    
    matrix3f matrix(
        rcos + u*u*(1-rcos),
        -w * rsin + u*v*(1-rcos),
        v * rsin + u*w*(1-rcos),
        
        w * rsin + v*u*(1-rcos),
        rcos + v*v*(1-rcos),
        -u * rsin + v*w*(1-rcos),

        -v * rsin + w*u*(1-rcos),
        u * rsin + w*v*(1-rcos),
        rcos + w*w*(1-rcos)    
    );
    return matrix;
}