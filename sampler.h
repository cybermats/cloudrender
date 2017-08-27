#pragma once

class sampler
{
private:
    int _counter;
public:
    sampler(int counter)
    : _counter(counter)
    {}

    bool done()
    {
        return (_counter-- == 0);
    }
};