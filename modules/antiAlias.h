
#pragma once
#include "global_components/skFilters.h"
#include <memory>
#include <vector>

class AntiAlias
{
public:
    
    void setup(double _sampleRate);

    void processBlock(float** buffer, size_t size);

private:

    float sampleRate;
    const float freq = 10000.0;
    const float q = 0.707;

    std::unique_ptr<skFilters> aaf = std::make_unique<skFilters>(lowpass);
};
