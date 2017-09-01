#pragma once

#include <vector>
#include <fstream>

#include "vecmath.h"

class imagebuffer
{
private:
    size_t _width;
    size_t _height;

    std::vector<color> _buffer;

public:
    imagebuffer(size_t width, size_t height)
    : _width(width), _height(height)
    , _buffer((size_t) width * height)
    {}

    void set(size_t x, size_t y, const color& v) {
        _buffer[y * _width + x] = v;
    }

    color& get(size_t x, size_t y) {
        return _buffer[y * _width + x];
    }

    size_t width() const {
        return _width;
    }

    size_t height() const {
        return _height;
    }


    void save(const char* filename) {
        std::ofstream f(filename, std::ofstream::out | std::ofstream::binary);
        f << "P6\n" << _width << " " << _height << "\n255\n";
        for (auto& c: _buffer) {
            char b[3];
            b[0] = c.r * 255;
            b[1] = c.g * 255;
            b[2] = c.b * 255;
            f.write(b, 3);
        }
        f.close();
    }

};