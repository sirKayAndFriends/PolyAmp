#include "midChannel.h"

void MID_Channel::setup(double _sampleRate)
{
    id = 1;

    sampleRate = _sampleRate;

    gainMultiplier = 20.0;

    triode1.outputGain = 0.7;
    triode1.waveSaturation = 1.0;
    triode1.outputLSFfreq = 40.0;
    triode1.outputHSFfreq = 8000.0;

    triode2.outputGain = 1.5;
    triode2.waveSaturation = 1.0;
    triode2.outputLSFfreq = 100.0;
    triode2.outputHSFfreq = 7000.0;

    triode3.outputGain = 1.5;
    triode3.waveSaturation = 1.0;
    triode3.outputLSFfreq = 100.0;
    triode3.outputHSFfreq = 6000.0;

    triode4.outputGain = 0.1;
    triode4.waveSaturation = 1.0;
    triode4.outputLSFfreq = 40.0;
    triode4.outputHSFfreq = 5000.0;

    triode1.setup(dcOffset[0], sampleRate);
    triode2.setup(dcOffset[1], sampleRate);
    triode3.setup(dcOffset[2], sampleRate);
    triode4.setup(dcOffset[3], sampleRate);
}

void MID_Channel::processBlock(float** buffer, size_t size)
{
    triode1.processBlock(buffer, size);

    volume.processBlock(buffer, size);

    triode2.processBlock(buffer, size);
    
    volume.processBlock(buffer, size);

    triode3.processBlock(buffer, size);

    triode4.processBlock(buffer, size);
}