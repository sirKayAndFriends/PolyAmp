#include "pentode.h"

Pentode::Pentode()
{

}

void Pentode::setup(double Fs)
{
    lossyIntegrator[0].setup(Fs, 5.0);
    lossyIntegrator[1].setup(Fs, 5.0);
}

void Pentode::update(float sag, float saturation)
{
    params.gain = sag;
    params.waveSaturation = saturation;
}

float Pentode::gridConduction(float input, float thresh)
{
    if (input > 0.0)
    {
        float clipDelta = input - thresh;

        clipDelta = fmax(clipDelta, 0.0);

        float compressionFactor = 0.4473253 + 0.5451584 * exp(-0.3241584 * clipDelta);

        return compressionFactor * input;
    }
    else
    {
        return input;
    }
}

void Pentode::processBlock(float* buffer, size_t size)
{
    for(size_t index = 0; index < size; index++)
    {
        float sample = buffer[index];

        //PIRKLE POWER AMP
        float in_pos = sample;
        float in_neg = -sample;

        in_pos = gridConduction(in_pos, 0.0);
        in_neg = gridConduction(in_neg, 0.0);

        float dcOffsetPos = lossyIntegrator[0].processAudioSample(in_pos);
        float dcOffsetNeg = lossyIntegrator[1].processAudioSample(in_neg);

        dcOffsetPos = fmin(dcOffsetPos, 0.0);
        dcOffsetNeg = fmin(dcOffsetNeg, 0.0);

        float out_pos = doPirkleWaveshaper(in_pos,
            params.waveSaturation,
            params.fixedBiasVoltage,
            dcOffsetPos * params.dcShiftCoeff);

        float out_neg = doPirkleWaveshaper(in_neg,
            params.waveSaturation,
            params.fixedBiasVoltage,
            dcOffsetPos * params.dcShiftCoeff);

        sample = out_pos - out_neg;

        sample *= params.gain;

        buffer[index] = sample;
    }
}

float Pentode::doPirkleWaveshaper(float input, float g, float fixedDC, float variableDC)
{
    input += fixedDC;
    input += variableDC;

    float out = 1.5 * atan(g * input) / atan(g);

    return out;
}
