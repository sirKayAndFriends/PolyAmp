#pragma once
#include <vector>
#include <memory>
#include "HiFi_LoFi_fftConvolver/FFTConvolver.h"
#include "cabIRs.h"
#define IR_SIZE 1024

class CabSimFFT
{
public:
    CabSimFFT();

    void setup(double sampleRate, int blockSize);

    void update(int newCab);

    void processBlock(float* buffer, size_t size);

    int numCabs = 0;

private:

    int _blockSize;
    double _sampleRate;

    bool ready = false;

    std::shared_ptr<impulseResponses> cabs = std::make_shared<impulseResponses>();

    std::unique_ptr<fftconvolver::FFTConvolver> convolver = std::make_unique<fftconvolver::FFTConvolver>();

};