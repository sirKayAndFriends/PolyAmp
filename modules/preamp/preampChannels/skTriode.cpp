//This algorithm is pretty much identical to Will Pirkle's triode code 
//in the addendum to the book "Designing Audio Effect Plugins in C++"
// 
// https://www.willpirkle.com/fx-book-bonus-material/chapter-19-addendum/
// 
//well worth a read!  Thanks muchly.
//I've cut it up a little to fit in with how I like to do things but it essentially the same

#include "skTriode.h"

SKTriode::SKTriode()
{

}


void SKTriode::setup(float dc, double Fs)
{
    sampleRate = Fs;

    outputLPFfreq = 7000.0;

    integratorFc = 5.0;

    clipPointPositive = 4.0;
    clipPointNegative = -1.5;
    gridThresh = 1.5;

    dcShiftCoeff = dc;

    lossyIntegrator.setup(Fs, integratorFc);

    outputLPFilter->calc(outputLPFfreq, 1.0, 0.707, sampleRate);

    outputLSFilter->calc(outputLSFfreq, -3.0, 1.0, sampleRate);
    outputHSFilter->calc(outputHSFfreq, -3.0, 1.0, sampleRate);

    debugVal = sampleRate;
}

void SKTriode::processBlock(float** buffer, size_t size)
{
    for(size_t index = 0; index < size; index++)
    {
        float sample = buffer[0][index];

        sample = gridConduction(sample, gridThresh);

        float dcOffset = lossyIntegrator.processAudioSample(sample);
        dcOffset = fmin(dcOffset, 0.0);

        sample = valveEmulation(
            sample,
            waveSaturation,
            gridThresh,
            dcOffset * dcShiftCoeff,
            clipPointPositive,
            clipPointNegative);

        sample = outputLSFilter->process(sample);
        
        sample = outputHSFilter->process(sample);

        sample = outputLPFilter->process(sample);

        sample *= -outputGain;

        buffer[0][index] = sample;
    }
}

float SKTriode::gridConduction(float input, float thresh)
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

float SKTriode::valveEmulation(float input, float k, float gridConductionThresh, float dcOffset, float clipPointPos, float clipPointNeg)
{
    input += dcOffset;
    float yn;

    if (input > gridConductionThresh)
    {
        if (input > clipPointPos)
        {
            yn = clipPointPos;
        }
        else
        {
            input -= gridConductionThresh;

            if (clipPointPos > 1.0)
            {
                input /= (clipPointPos - gridConductionThresh);
            }

            yn = input * (3.0 / 2.0) * (1.0 - (input * input) / 3.0);

            yn *= (clipPointPos - gridConductionThresh);

            yn += gridConductionThresh;
        }
    }

    else if (input > 0.0)
    {
        yn = input;
    }

    else
    {
        if (input < clipPointNeg)
        {
            yn = clipPointNeg;
        }

        else
        {
            if (clipPointNeg < -1.0)
            {
                input /= fabs(clipPointNeg);
            }

            yn = tanh(k * input) / tanh(k);

            yn *= fabs(clipPointNeg);
        }
    }

    return yn;
}


