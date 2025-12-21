#include "tone.h"

Tone::Tone()
{

}

std::shared_ptr<Tone::toneData> Tone::setup(double _sampleRate)
{
    sampleRate = _sampleRate;
    update();

    return params;
}

void Tone::update()
{
    bassGain = scalePin(gainMin, gainMax, params->bassGain);
    trebleGain = scalePin(gainMin, gainMax, params->trebleGain);
    midGain = scalePin(gainMin, gainMax, params->midGain);
    midFreq = scalePin(midFreqMin, midFreqMax, params->midFreq);

    bassTone->calc(bassFreq, bassGain, slope, sampleRate);
    midTone->calc(midFreq, midGain, q, sampleRate);
    trebleTone->calc(trebleFreq, trebleGain, slope, sampleRate);
}

void Tone::processBlock(float* buffer, size_t size)
{
    bassTone->processBlock(buffer, size);

    midTone->processBlock(buffer, size);

    trebleTone->processBlock(buffer, size);
}
