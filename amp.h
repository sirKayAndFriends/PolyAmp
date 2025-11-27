#pragma once
#include "includes.h"

class Amp
{
public:

    Amp();

    void setup(double sampleRate, int blockSize);

    void updateParameters();

    void processBlock(InputBuffer input, OutputBuffer output, size_t size);

    void attachPresetStorage(float* preset1, float* preset2, float* preset3);

    std::vector<float> knobValues;
    std::vector<bool> channelPins;
    std::vector<bool> cabPins;
    std::vector<int> presetPins = {0, 0, 0};

private:

    //handles all of the parameter stuff from the hardware
    //raw pin values, change listener, presets etc
    std::shared_ptr<HardwareInterface> ampParams = std::make_shared<HardwareInterface>();

    //indicates which module to update
    //preamp, poweramp and outputvolume updated together under "channel"
    //preamp gain updated separately
    std::vector<bool> moduleUpdateFlags = { true, true, true, true, true, true};
    
    //module objects
    Boost boost;
    Preamp pre;
    PowerAmp power;
    Tone tonestack;
    CabSimFFT cabsim;
    Output outputVolume;

    AntiAlias aaf1;
    AntiAlias aaf2;
    
    //accessible parameter structs
    std::shared_ptr<Boost::boostData> boostParams;
    std::shared_ptr<Preamp::preampData> preampParams;
    std::shared_ptr<PowerAmp::powerData> powerParams;
    std::shared_ptr<Tone::toneData> toneParams;
    std::shared_ptr<Output::outputData> volumeParams;

    //makes the pin names easier to read
    typedef HardwareInterface pin;
};
