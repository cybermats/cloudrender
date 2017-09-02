#pragma once

class sampler
{
private:
    int _counter;
    int _step;
    int _break;
    int _progress;
public:
    sampler(int counter)
    : _counter(counter)
    {
        _step = _counter / 10;
        _break = _counter;
        _progress = 0;

    }

    bool done()
    {
        if (_counter < _break) {
            std::cout << "[" << _progress << "%]" << std::endl;
            _progress += 10;
            _break -= _step;
        }

        return (_counter-- == 0);
    }
};