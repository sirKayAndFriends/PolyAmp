#include "amp.h"

Amp::Amp()
{
}

void Amp::setup(double sampleRate, int blockSize)
{
    HWI.setup(pinValues, paramValues, moduleUpdateFlags, presetPins);

    boostParams = boost.setup(sampleRate);
    preampParams = pre.setup(sampleRate);
    powerParams = power.setup(sampleRate);
    toneParams = tonestack.setup(sampleRate);
    volumeParams = outputVolume.setup();

    cabsim.setup(sampleRate, blockSize);

    aaf1.setup(sampleRate);
    aaf2.setup(sampleRate);
}

void Amp::updateParameters
(        
    float boostGain, float boostFrequency, float boostBass,
    float distortion,
    float treble, float mid, float bass, float voice,
    float masterVolume,
    int channelNumber,
    int cabNumber
)
{
    pinValues->at(boostAmt) = boostGain;
    pinValues->at(boostFreq) = boostFrequency;
    pinValues->at(boostCut) = boostBass;
    pinValues->at(overdrive) = distortion;
    pinValues->at(toneBass) = bass;
    pinValues->at(toneMid) = mid;
    pinValues->at(toneTreble) = treble;
    pinValues->at(toneVoice) = voice;
    pinValues->at(volume) = masterVolume;
    pinValues->at(channel) = (float) channelNumber;
    pinValues->at(cabslot) = (float) cabNumber;

    // HWI.presetCheck();

    //check if hardware values have changed
    HWI.parameterCheck();

    // debugVal = HWI.debugVal;
    debugVal = HWI.debugVal;

    if(moduleUpdateFlags->at(BoostModule))
    {
        boostParams->peakGain = paramValues->at(boostAmt);
        boostParams->peakFreq = paramValues->at(boostFreq);
        boostParams->bassGain = paramValues->at(boostCut);

        boost.update();

        moduleUpdateFlags->at(BoostModule) = false;
    }

    if(moduleUpdateFlags->at(PreampModule))
    {
        preampParams->gainKnobValue = paramValues->at(overdrive); 
        pre.update();

        moduleUpdateFlags->at(PreampModule) = false;
    }

    if(moduleUpdateFlags->at(TonestackModule))
    {
        toneParams->bassGain = paramValues->at(toneBass);
        toneParams->midGain = paramValues->at(toneMid);
        toneParams->trebleGain = paramValues->at(toneTreble);
        toneParams->midFreq = paramValues->at(toneVoice);

        tonestack.update();

        moduleUpdateFlags->at(TonestackModule) = false;
    }

    if(moduleUpdateFlags->at(ChannelModule))
    {
        preampParams->activeChannel = static_cast<int>(paramValues->at(channel));
        powerParams->activeChannel = preampParams->activeChannel;
        volumeParams->activeChannel = preampParams->activeChannel;

        power.update();

        moduleUpdateFlags->at(ChannelModule) = false;
    }

    if(moduleUpdateFlags->at(CabsimModule))
    {
        cabsim.update(paramValues->at(cabslot)); 

        moduleUpdateFlags->at(CabsimModule) = false;
    }

    if(moduleUpdateFlags->at(VolumeModule))
    {
        volumeParams->volume = paramValues->at(volume);
        outputVolume.update(); 

        moduleUpdateFlags->at(VolumeModule) = false;
    }
}

void Amp::processBlock(const float* input, float* output, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        output[i] = input[i];
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
}

void Amp::processBlock(float* input, size_t size)
{
    #ifdef BOOST
    {
        boost.processBlock(input, size); 
    }
    #endif

    pre.processBlock(input, size);

    aaf1.processBlock(input, size);

    power.processBlock(input, size);

    aaf2.processBlock(input, size);

    tonestack.processBlock(input, size);

    #ifdef CABSIM
    {
        cabsim.processBlock(input, size);
    }
    #endif

    outputVolume.processBlock(input, size);
}

void Amp::attachPresetStorage(float* preset1, float* preset2, float* preset3)
{
    HWI.preset1 = preset1;
    HWI.preset2 = preset2;
    HWI.preset3 = preset3;
}

void Amp::updatePresets(int presetSlot, int command)
{
    presetPins->at(presetSlot) = command;
}

int Amp::queryPresets(int presetSlot)
{
    return presetPins->at(presetSlot);
}
