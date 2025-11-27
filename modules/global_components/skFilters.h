#pragma once
#include "math.h"
#include <vector>
#define PI 3.14159265358979311600

enum type
{
    highpass,
    lowpass,
    peak,
    highshelf,
    lowshelf
};

class skFilters
{
public:
    skFilters(int type);
    
    //void calc(float& freq, float& gain, float& q, float Fs);
    void calc(float freq, float gain, float q, float Fs);

    float process(float input);

    void processBlock(float** buffer, size_t size);

private:
    int filterType;
    float b0, b1, b2, a0, a1, a2;
    float z1, z2;
};