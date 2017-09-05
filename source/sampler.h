#pragma once

#include <mutex>

class sampler
{
private:
    int _counter;
    int _step;
    int _break;
    int _progress;
    constexpr static int step_count = 20;
    std::mutex _mtx;
    
public:
    sampler(int counter)
    : _counter(counter)
    {
        _step = _counter / step_count;
        _break = _counter;
        _progress = 0;

    }

    bool done()
    {
      std::lock_guard<std::mutex> lg(_mtx);
        if (_counter < _break) {
            std::cout << "[" << _progress << "%]" << std::endl;
            _progress += 100 / step_count;
            _break -= _step;
        }

        return (_counter-- <= 0);
    }
};
