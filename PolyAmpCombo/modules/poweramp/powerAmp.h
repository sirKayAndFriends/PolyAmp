
#pragma once
#include "pentode.h"
#include <memory>
#include <vector>

class PowerAmp
{
public:

    struct powerData
    {
        //gain boost between preamp and poweramp
        //makes more difference in the higher channel
        std::vector<float> sag =
        {
            1.0, 2.0, 2.0
        };

        std::vector<float> saturation =
        {
            1.2, 2.0, 3.0
        };

        int activeChannel = 0;
    };

    PowerAmp();

    std::shared_ptr<powerData> setup(double _sampleRate);

    void update();

    void processBlock(float* buffer, size_t size);

private:

    double sampleRate;

    std::shared_ptr<powerData> params = std::make_shared<powerData>();

    std::unique_ptr<Pentode> pentodes = std::make_unique<Pentode>();
};


