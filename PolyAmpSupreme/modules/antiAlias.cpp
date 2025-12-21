#include "antiAlias.h"

void AntiAlias::setup(double _sampleRate)
{
    sampleRate = _sampleRate;

    aaf->calc(freq, 1.0, q, sampleRate);
}

void AntiAlias::processBlock(float* buffer, size_t size)
{
    aaf->processBlock(buffer, size);
}

