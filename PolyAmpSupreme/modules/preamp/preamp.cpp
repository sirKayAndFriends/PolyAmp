#include "preamp.h"

Preamp::Preamp()
{

}

std::shared_ptr<Preamp::preampData> Preamp::setup(double _sampleRate)
{
    sampleRate = _sampleRate;

    loChannel.setup(sampleRate);
    midChannel.setup(sampleRate);
    hiChannel.setup(sampleRate);

    return params;
}

void Preamp::update()
{
    loChannel.update(params->gainKnobValue);
    midChannel.update(params->gainKnobValue);
    hiChannel.update(params->gainKnobValue);
}

void Preamp::processBlock(float* buffer, size_t size)
{
    channels[params->activeChannel]->processBlock(buffer, size);
}
