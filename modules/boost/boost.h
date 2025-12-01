
#pragma once
#include "../global_components/skFilters.h"
#include <memory>
#include <vector>

class Boost
{
public:

    struct boostData
    {
        float peakGain;
        float peakFreq;
        float bassGain;
    };

    Boost();
    
    std::shared_ptr<boostData> setup(double _sampleRate);

    void update();

    void processBlock(float* buffer, size_t size);

private:

    double sampleRate;
    const float outputLPfreq = 5000.0;
    const float q = 0.707;

    float peakFreq = 700.0;
    float peakGain = 0.0;

    //boost voice range
    const float peakMin = 700.0;
    const float peakMax = 2500.0;
    const float peakGainMax = 24.0;
    const float peakGainMin = 0.0;

    const float bassFreq = 100.0;
    float bassGain = 0.0;

    const float bassGainMax = 12.0;
    const float bassGainMin = -12.0;

    std::shared_ptr<boostData> params = std::make_shared<boostData>();

    std::unique_ptr<skFilters> bassCut = std::make_unique<skFilters>(lowshelf);
    std::unique_ptr<skFilters> midBoost = std::make_unique<skFilters>(highshelf);
    std::unique_ptr<skFilters> outputLP = std::make_unique<skFilters>(lowpass);

    inline float scalePin(float min, float max, float value)
    {
        return value * (max - min) + min;
    }
};
