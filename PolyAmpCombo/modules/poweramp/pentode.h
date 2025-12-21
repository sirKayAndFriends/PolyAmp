#pragma once
#include "math.h"
#include "../global_components/pirkleZVA.h"
#include <vector>

struct powerAmpParameters
{  
    double gain = 1.0;

    double fixedBiasVoltage = -1.5;
    double clipPointPositive = 1.5;

    double dcShiftCoeff = 0.5;
    double waveSaturation = 1.2;
};

class Pentode
{
public:
    Pentode();

    void setup(double Fs);

    void update(float sag, float saturation);
    
    void processBlock(float* buffer, size_t size);

    float doPirkleWaveshaper(float input, float g, float fixedDC, float variableDC);

    float gridConduction(float input, float thresh);

private:

    powerAmpParameters params;
    ZVAFilter lossyIntegrator[2];
};
