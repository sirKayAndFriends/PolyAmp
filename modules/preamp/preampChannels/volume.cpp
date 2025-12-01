#include "volume.h"

void Volume::processBlock(float* buffer, size_t size)
{
    for(size_t index = 0; index < size; index++)
    {
        buffer[index] *= gain;
    }
}

void Volume::update(float volume)
{
    gain = volume;
}