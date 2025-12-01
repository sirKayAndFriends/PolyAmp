#pragma once
#include "../global_components/skFilters.h"
#include <memory>
#include <vector>

class Tone
{
public:
    struct toneData
    {
        float bassGain;
        float trebleGain;
        float midGain;
        float midFreq;
    };

    Tone();

    std::shared_ptr<toneData> setup(double _sampleRate);

    void update();

    void processBlock(float* buffer, size_t size);
    
private:

    float sampleRate;
    
    const float trebleFreq = 2000.0;
    const float bassFreq = 250.0;

    float midFreq = 400.0;

    float trebleGain = 0.0;
    float bassGain = 0.0;
    float midGain = 0.0;

    //scaling limits
    const float gainMin = -12.0;
    const float gainMax = 12.0;

    //the voice knob range!
    const float midFreqMin = 400.0;
    const float midFreqMax = 850.0;

    const float slope = 1.0;
    const float q = 0.66;

    std::shared_ptr<toneData> params = std::make_shared<toneData>();

    std::unique_ptr<skFilters> bassTone = std::make_unique<skFilters>(lowshelf);
    std::unique_ptr<skFilters> midTone = std::make_unique<skFilters>(peak);
    std::unique_ptr<skFilters> trebleTone = std::make_unique<skFilters>(highshelf);

    inline float scalePin(float min, float max, float value)
    {
        return value * (max - min) + min;
    }
};
