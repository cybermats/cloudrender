#pragma once

#include <vector>
#include <fstream>
#include <mutex>
#include <cmath>

#include "vecmath.h"

class imagebuffer
{
 private:
  size_t _width;
  size_t _height;

  std::vector<color> _buffer;

  std::mutex _mtx;

 public:
 imagebuffer(size_t width, size_t height)
   : _width(width), _height(height)
    , _buffer((size_t) width * height)
    {}

  void set(size_t x, size_t y, const color& v) {
    std::lock_guard<std::mutex> lg(_mtx);
    _buffer[y * _width + x] = v;
  }

  color& get(size_t x, size_t y) {
    std::lock_guard<std::mutex> lg(_mtx);
    return _buffer[y * _width + x];
  }

  void inc(size_t x, size_t y, const color& v) {
    std::lock_guard<std::mutex> lg(_mtx);
    _buffer[y * _width + x] += v;
  }

  size_t width() const {
    return _width;
  }

  size_t height() const {
    return _height;
  }


  void save(const char* filename) {
    std::lock_guard<std::mutex> lg(_mtx);
    auto max = 0.f;
    for (auto& c: _buffer) {
      if (c.a > max)
	max = c.a;
    }
      
    std::ofstream f(filename, std::ofstream::out | std::ofstream::binary);
    f << "P6\n" << _width << " " << _height << "\n255\n";
    for (auto& c: _buffer) {
      char cb[3];
      constexpr static double gamma = 0.5;
      auto r = (double)c.r / max;
      auto g = (double)c.g / max;
      auto b = (double)c.b / max;
      r = std::pow(r, gamma);
      g = std::pow(g, gamma);
      b = std::pow(b, gamma);
      cb[0] = r * 255;
      cb[1] = g * 255;
      cb[2] = b * 255;
      f.write(cb, 3);
    }
    f.close();
  }

};
