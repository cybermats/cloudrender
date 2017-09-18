#pragma once

#include <vector>
#include <fstream>
#include <mutex>
#include <cmath>

#include "vecmath.h"
#include "color.h"

class imagebuffer_t
{
 private:
  size_t _width;
  size_t _height;

  std::vector<color_t> _buffer;

  std::mutex _mtx;

 public:
 imagebuffer_t(size_t width, size_t height)
   : _width(width), _height(height)
    , _buffer((size_t) width * height)
    {}

  void set(size_t x, size_t y, const color_t& v) {
    std::lock_guard<std::mutex> lg(_mtx);
    _buffer[y * _width + x] = v;
  }

  color_t& get(size_t x, size_t y) {
    std::lock_guard<std::mutex> lg(_mtx);
    return _buffer[y * _width + x];
  }

  void inc(size_t x, size_t y, const color_t& v) {
    std::lock_guard<std::mutex> lg(_mtx);
    _buffer[y * _width + x] += v;
  }

  void add(float u, float v, const color_t& c) {
    auto fx = u * _width;
    auto fy = v * _height;
    auto x = (int)fx;
    auto y = (int)fy;
    auto xf = fx - x;
    auto yf = fy - y;
    _buffer[y * _width + x] += ((1.f - xf) * (1.f - yf) * c);
    if (x < _width-1)
      _buffer[y * _width + x + 1] = (xf * (1.f - yf) * c);
    if (y < _height-1)
      _buffer[(y+1) * _width + x] = ((1.f - xf) * yf * c);
    if (y < _height-1 && x < _width-1)
      _buffer[(y+1) * _width + x + 1] = (xf * yf * c);
     
    
  }

  size_t width() const {
    return _width;
  }

  size_t height() const {
    return _height;
  }


  void save(const char* filename, double gamma) {
    std::lock_guard<std::mutex> lg(_mtx);
    auto max = 0.f;
    for (auto& c: _buffer) {
      if (c.intensity() > max)
	max = c.intensity();
    }
      
    std::ofstream f(filename, std::ofstream::out | std::ofstream::binary);
    f << "P6\n" << _width << " " << _height << "\n255\n";
    for (auto& c: _buffer) {
      char cb[3];
      auto r = (double)c.r / max;
      auto g = (double)c.g / max;
      auto b = (double)c.b / max;
      r = std::pow(r, 1./gamma);
      g = std::pow(g, 1./gamma);
      b = std::pow(b, 1./gamma);
      cb[0] = r * 255;
      cb[1] = g * 255;
      cb[2] = b * 255;
      f.write(cb, 3);
    }
    f.close();
  }

};
