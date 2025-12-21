#include "cabsim_fft.h"

CabSimFFT::CabSimFFT()
{

}

void CabSimFFT::setup(double sampleRate, int blockSize)
{
    _sampleRate = sampleRate;
    _blockSize = blockSize;

    numCabs = cabs->numCabs;
}

void CabSimFFT::update(int newCab)
{  
    ready = false;
    
    if(newCab < cabs->numCabs && cabs->cabPointers[newCab] != nullptr)
    {
        convolver->init(_blockSize, cabs->cabPointers[newCab], IR_SIZE);

        ready = true;
    }
}

void CabSimFFT::processBlock(float* buffer, size_t size)
{
    if(ready)
    {
        convolver->process(buffer, buffer, size);
    }
}