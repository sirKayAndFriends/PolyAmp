#include "amp.h"

Amp::Amp()
{
}

void Amp::setup(double sampleRate, int blockSize)
{
    ampParams->setup(sampleRate, blockSize, &moduleUpdateFlags);

    boostParams = boost.setup(sampleRate);
    preampParams = pre.setup(sampleRate);
    powerParams = power.setup(sampleRate);
    toneParams = tonestack.setup(sampleRate);
    volumeParams = outputVolume.setup();

    cabsim.setup(sampleRate, ampParams->dsy_blockSize);

    aaf1.setup(sampleRate);
    aaf2.setup(sampleRate);

    knobValues.resize(NUM_ADC_PARAMS);
    cabPins.resize(cabsim.numCabs);
    channelPins.resize(NUM_CHANNELS);

    updateParameters();
}

void Amp::updateParameters()
{
    ampParams->updatePresets(presetPins);

    //check if hardware values have changed
    ampParams->parameterCheck(knobValues, channelPins, cabPins);

    if(moduleUpdateFlags[BoostModule])
    {
        boostParams->peakGain = ampParams->paramValues[boostAmt];
        boostParams->peakFreq = ampParams->paramValues[boostFreq];
        boostParams->bassGain = ampParams->paramValues[boostCut];

    
        boost.update();

        moduleUpdateFlags[BoostModule] = false;
    }

    if(moduleUpdateFlags[PreampModule])
    {
        preampParams->gainKnobValue = ampParams->paramValues[overdrive]; 
        pre.update();

        moduleUpdateFlags[PreampModule] = false;
    }

    if(moduleUpdateFlags[TonestackModule])
    {
        toneParams->bassGain = ampParams->paramValues[toneBass];
        toneParams->midGain = ampParams->paramValues[toneMid];
        toneParams->trebleGain = ampParams->paramValues[toneTreble];
        toneParams->midFreq = ampParams->paramValues[toneVoice];

        tonestack.update();

        moduleUpdateFlags[TonestackModule] = false;
    }

    if(moduleUpdateFlags[ChannelModule])
    {
        preampParams->activeChannel = static_cast<int>(ampParams->paramValues[channel]);
        powerParams->activeChannel = preampParams->activeChannel;
        volumeParams->activeChannel = preampParams->activeChannel;

        power.update();

        moduleUpdateFlags[ChannelModule] = false;
    }

    if(moduleUpdateFlags[CabsimModule])
    {
        cabsim.update(ampParams->paramValues[cabslot]); 

        moduleUpdateFlags[CabsimModule] = false;
    }

    if(moduleUpdateFlags[VolumeModule])
    {
        volumeParams->volume = ampParams->paramValues[volume];
        outputVolume.update(); 

        moduleUpdateFlags[VolumeModule] = false;
    }
}

void Amp::processBlock(InputBuffer input, OutputBuffer output, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        output[0][i] = input[0][i];
    }   
    
    #ifdef BOOST
    {
        boost.processBlock(output, size); 
    }
    #endif

    pre.processBlock(output, size);

    aaf1.processBlock(output, size);

    power.processBlock(output, size);

    aaf2.processBlock(output, size);

    tonestack.processBlock(output, size);

    #ifdef CABSIM
    {
        cabsim.processBlock(output, size);
    }
    #endif

    outputVolume.processBlock(output, size);

    #ifdef STEREO
    {
        output[1] = output[0];
    }
    #endif
}

void Amp::attachPresetStorage(float* preset1, float* preset2, float* preset3)
{
    ampParams->preset1 = preset1;
    ampParams->preset2 = preset2;
    ampParams->preset3 = preset3;
}
