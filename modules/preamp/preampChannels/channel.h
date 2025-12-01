#pragma once
#include "volume.h"
#include <vector>

class Channel
{
public:
    
    virtual void setup(double _sampleRate) = 0;

    void update(float gain)
    {
        volume.update(gain * gainMultiplier);
    };

    virtual void processBlock(float* buffer, size_t size) = 0;

    float debugVal;
    
    Volume volume;
    
    double sampleRate;

    float gainMultiplier;

    int id;

    const std::vector<double> dcOffset
    {
       1.0,
       0.2,
       0.3,
       0.5
    };
};