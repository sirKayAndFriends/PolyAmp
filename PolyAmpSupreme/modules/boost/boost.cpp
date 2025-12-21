#include "boost.h"

Boost::Boost()
{

}

std::shared_ptr<Boost::boostData> Boost::setup(double _sampleRate)
{
    sampleRate = _sampleRate;

    outputLP->calc(outputLPfreq, 1.0, q, sampleRate);

    update();

    return params;
}

void Boost::update()
{   
    bassGain = scalePin(bassGainMin, bassGainMax, params->bassGain);
    peakFreq = scalePin(peakMin, peakMax, params->peakFreq);
    peakGain = scalePin(peakGainMin, peakGainMax, params->peakGain);

    bassCut->calc(bassFreq, bassGain, 1.0, sampleRate);
    midBoost->calc(peakFreq, peakGain, 1.0, sampleRate);
}

void Boost::processBlock(float* buffer, size_t size)
{
    bassCut->processBlock(buffer, size);
    midBoost->processBlock(buffer, size);
    outputLP->processBlock(buffer, size);
}

