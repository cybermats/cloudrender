#pragma once

#include <mutex>
#include "logger.h"

class sampler
{
private:
    int _counter;
    int _step;
    int _break;
    int _progress;
    bool _show_progress;
    constexpr static int step_count = 20;
    std::mutex _mtx;
    
public:
 sampler(int counter, bool show_progress = true)
    : _counter(counter)
      , _show_progress(show_progress)
    {
        _step = _counter / step_count;
        _break = _counter;
        _progress = 0;

    }

    

    bool done()
    {
      std::lock_guard<std::mutex> lg(_mtx);
        if (_counter < _break && _show_progress) {
	  LOG_INFO << "[" << _progress << "%]";
            _progress += 100 / step_count;
            _break -= _step;
        }

        return (_counter-- <= 0);
    }
};
