#pragma once
#include "channel.h"
#include "skTriode.h"
#include <vector>

class LO_Channel : public Channel
{
public:
    
    void setup(double _sampleRate) override;

    void processBlock(float* buffer, size_t size) override;

    int id = 0;

private:
    
    SKTriode triode1, triode2, triode3;
};