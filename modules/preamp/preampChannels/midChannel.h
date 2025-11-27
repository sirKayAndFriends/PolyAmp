#pragma once
#include "skTriode.h"
#include "channel.h"
#include <vector>

class MID_Channel : public Channel
{
public:
    
    void setup(double _sampleRate) override;

    void processBlock(float** buffer, size_t size) override;

    int id = 1;

private:
    
    SKTriode triode1, triode2, triode3, triode4;
};