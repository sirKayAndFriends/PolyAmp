#include "output.h"

void Output::update()
{
    for(size_t i = 0; i < channelOutputGain.size(); i++)
    {
        channelOutputGain[i] = scalePin(0.0f, channelOutputMax[i], params->volume);
    }
}

std::shared_ptr<Output::outputData> Output::setup()
{
    return params;
}

void Output::processBlock(float* buffer, size_t size)
{
    for(size_t index = 0; index < size; index++)
    {
        buffer[index] *= channelOutputGain[params->activeChannel];
    }
}