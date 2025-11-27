#pragma once
#include <cstdlib>

class Volume
{
public:

    void processBlock(float** buffer, size_t size);

    void update(float volume);

    float gain;

private:
};