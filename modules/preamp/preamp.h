
#pragma once
#include "preampChannels/loChannel.h"
#include "preampChannels/hiChannel.h"
#include "preampChannels/midChannel.h"
#include <memory>
#include <vector>

class preampDataObject
{
public:
    preampDataObject()
    {
        
    }

    int numTriodes;

    std::vector<double> outputGain;

    std::vector<double> outputLS;

    std::vector<double> outputHS;

    double saturation;
};

class Preamp
{
public:

    struct preampData
    {
        int activeChannel = 0;

        float gainKnobValue = 0.1;
    };

    Preamp();

    std::shared_ptr<preampData> setup(double _sampleRate);

    void update();

    void processBlock(float* buffer, size_t size);

private:

    double sampleRate;

    LO_Channel loChannel;
    MID_Channel midChannel;
    HI_Channel hiChannel;

    std::vector<Channel*> channels = 
    {
        &loChannel,
        &midChannel,
        &hiChannel
    };

    std::shared_ptr<preampData> params = std::make_shared<preampData>();
};
