#pragma once
#include "skTriode.h"
#include "channel.h"
#include <vector>

class HI_Channel : public Channel
{
public:
    
    void setup(double _sampleRate) override;

    void processBlock(float* buffer, size_t size) override;

private:
    
    SKTriode triode1, triode2, triode3, triode4;
};