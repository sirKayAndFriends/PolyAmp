//https://www.gcradix.org/post/introduction-the-biquad-filter

#include "skFilters.h"

skFilters::skFilters(int type)
{
    b0 = 1.0f;
    b1 = b2 = 0.0f;
    a0 = a1 = a2 = 0.0f;
    z1 = z2 = 0.0f;

    filterType = type;
}

void skFilters::calc(float freq, float gain, float q, float Fs)
{
    const float v = pow(10.0, (fabs(gain) / 20.0));
    const float k = tan(PI * (freq / Fs));
    const float kSquare = k * k;

    switch (filterType)
    {

        case lowpass: 
        {
            const float norm = 1.0 / (1.0 + k / q + kSquare);
            a0 = kSquare * norm;
            a1 = 2.0 * a0;
            a2 = a0;
            b1 = (2.0 * (kSquare - 1.0) * norm);
            b2 = ((1.0 - k / q + kSquare) * norm);

            break;
        }

        case highpass:
        {
            const float norm = 1.0 / (1.0 + k / q + kSquare);
            a0 = 1.0 * norm;
            a1 = -2.0 * a0;
            a2 = a0;
            b1 = (2.0 * (kSquare - 1.0) * norm);
            b2 = norm * (1.0 - k / q + kSquare);

            break;
        }

        case peak:
        {
            const float x1 = k / q;
            const float x2 = k * v / q;

            if (gain >= 0)
            {
                const float norm = 1.0/ (1.0 + x1 + kSquare);
                a0 = static_cast<float>((1.0 + x2 + kSquare) * norm);
                a1 = static_cast<float>(2.0 * (kSquare - 1) * norm);
                a2 = static_cast<float>((1.0 - x2 + kSquare) * norm);
                b1 = a1;
                b2 = static_cast<float>((1.0 - x1 + kSquare) * norm);
            }
            else
            {
                float const norm = 1 / (1 + x2 + kSquare);
                a0 = static_cast<float>((1 + x1 + kSquare) * norm);
                a1 = static_cast<float>((2 * (kSquare - 1)) * norm);
                a2 = static_cast<float>((1 - x1 + kSquare) * norm);
                b1 = a1;
                b2 = static_cast<float>((1 - x2 + kSquare) * norm);
            }

            break;
        }

        case highshelf:
        {
            const float x1 = sqrt(2.0 * v) * k;
            const float x2 = sqrt(2.0) * k;

            if (gain >= 0)
            {
                float const norm = 1 / (1 + x2 + kSquare);
               a0 = static_cast<float>((v + x1 + kSquare) * norm);
               a1 = static_cast<float>(2.0 * (kSquare - v) * norm);
               a2 = static_cast<float>((v - x1 + kSquare) * norm);
               b1 = static_cast<float>(2.0 * (kSquare - 1) * norm);
               b2 = static_cast<float>((1.0 - x2 + kSquare) * norm);
            }
            else
            {
                float const norm = 1.0 / (v + x1 + kSquare);
               a0 = static_cast<float>((1.0 + x1 + kSquare) * norm);
               a1 = static_cast<float>(2.0 * (kSquare - 1.0) * norm);
               a2 = static_cast<float>((1.0 - x1 + kSquare) * norm);
               b1 = static_cast<float>(2.0 * (kSquare - v) * norm);
               b2 = static_cast<float>((v - x1 + kSquare) * norm);
            }

            break;
        }

        case lowshelf:
        {
            const float x3 = sqrt(2.0) * k;
            const float x1 = sqrt(2.0 * v) * k;
            const float x2 = v * kSquare;

            if (gain >= 0)
            {
                float const norm = 1.0 / (1.0 + x3 + kSquare);
                a0 = static_cast<float>((1.0 + x1 + x2) * norm);
                a1 = static_cast<float>(2.0 * (x2 - 1.0) * norm);
                a2 = static_cast<float>((1.0 - x1 + x2) * norm);
                b1 = static_cast<float>(2.0 * (kSquare - 1.0) * norm);
                b2 = static_cast<float>((1.0 - x3 + kSquare) * norm);
            }
            else
            {
                float const norm = 1.0 / (1.0 + x1 + x2);
                a0 = static_cast<float>((1.0 + x3 + kSquare) * norm);
                a1 = static_cast<float>(2.0 * (kSquare - 1.0) * norm);
                a2 = static_cast<float>((1.0 - x3 + kSquare) * norm);
                b1 = static_cast<float>(2.0 * (x2 - 1.0) * norm);
                b2 = static_cast<float>((1.0 - x1 + x2) * norm);
            }

            break;
        }
    }
}

float skFilters::process(float input)
{
    float output = input * a0 + z1;
    z1 = input * a1 + z2 - b1 * output;
    z2 = input * a2 - b2 * output;

    return output;
}

void skFilters::processBlock(float** buffer, size_t size)
{
    for(size_t index = 0; index < size; index++)
    {
        float sample = buffer[0][index];

        float output = sample * a0 + z1;
        z1 = sample * a1 + z2 - b1 * output;
        z2 = sample * a2 - b2 * output;

        buffer[0][index] = output;
    }
}