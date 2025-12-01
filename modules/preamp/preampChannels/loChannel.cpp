#include "loChannel.h"

void LO_Channel::setup(double _sampleRate)
{
    id = 0;
    
    sampleRate = _sampleRate;

    gainMultiplier = 10.0;

    triode1.outputGain = 0.7;
    triode1.waveSaturation = 0.5;
    triode1.outputLSFfreq = 40.0;
    triode1.outputHSFfreq = 10000.0;

    triode2.outputGain = 1.5;
    triode2.waveSaturation = 0.5;
    triode2.outputLSFfreq = 40.0;
    triode2.outputHSFfreq = 8000.0;

    triode3.outputGain = 0.1;
    triode3.waveSaturation = 0.5;
    triode3.outputLSFfreq = 40.0;
    triode3.outputHSFfreq = 7000.0;

    triode1.setup(dcOffset[0], sampleRate);
    triode2.setup(dcOffset[1], sampleRate);
    triode3.setup(dcOffset[2], sampleRate);
}

void LO_Channel::processBlock(float* buffer, size_t size)
{
    triode1.processBlock(buffer, size);

    volume.processBlock(buffer, size);

    triode2.processBlock(buffer, size);

    volume.processBlock(buffer, size);

    triode3.processBlock(buffer, size);

    debugVal = volume.gain;
}