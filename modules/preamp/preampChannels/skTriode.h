
#pragma once
#include "math.h"
#include "../../global_components/skFilters.h"
#include "../../global_components/pirkleZVA.h"
#include <memory>
#include <vector>

class SKTriode
{
public:
    SKTriode();

    void setup(float dc, double Fs);
    
    void processBlock(float* buffer, size_t size);

    //the compression part of the waveshaping
    float gridConduction(float input, float thresh);

    //the actual overdrive waveshaping!
    float valveEmulation(float input, float k, float gridConductionThresh, float dcOffset, float clipPointPos, float clipPointNeg);

    float debugVal;
    float outputHSFfreq = 10;
    float outputLSFfreq;
    float waveSaturation;
    float outputGain;

private:

    double sampleRate;

    float integratorFc;
    float clipPointPositive;
    float clipPointNegative;
    float gridThresh;
    float outputLPFfreq;
    float dcShiftCoeff;

    ZVAFilter lossyIntegrator;

    std::unique_ptr<skFilters> outputHSFilter = std::make_unique<skFilters>(highshelf);
    std::unique_ptr<skFilters> outputLSFilter = std::make_unique<skFilters>(lowshelf);
    std::unique_ptr<skFilters> outputLPFilter = std::make_unique<skFilters>(lowpass);
};

