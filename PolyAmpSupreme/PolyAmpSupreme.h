#pragma once
#include "core/includes.h"

class Amp
{
public:

    Amp();

    void setup(double sampleRate, int blockSize);

    void updateParameters(      
        float boostGain, float boostFrequency, float boostBass,
        float distortion,
        float treble, float mid, float bass, float voice,
        float masterVolume,
        int channelNumber,
        int cabNumber 
    );

    //update the preset situation
    //command MUST be 1, 0 or -1
    //see documentation
    void updatePresets(int presetSlot, int command);

    //check value of preset slots
    int queryPresets(int presetSlot);

    //process audio block
    //for systems that have a constant input buffer
    //and a separate output buffer
    void processBlock(const float* input, float* output, size_t size);

    //process audio block
    //for systems that just have a single audio buffer
    void processBlock(float* input, size_t size);

    //pointers to your preset vectors!
    void attachPresetStorage(float* preset1, float* preset2, float* preset3);

private:

    //raw hardware values
    std::shared_ptr<std::vector<float>> pinValues = std::make_shared<std::vector<float>>();

    //processor values
    //the values the amp processor reads
    std::shared_ptr<std::vector<float>> paramValues = std::make_shared<std::vector<float>>();

    //indicates which module to update
    //preamp, poweramp and outputvolume updated together under "channel"
    //preamp gain updated separately
    std::shared_ptr<std::vector<bool>> moduleUpdateFlags = std::make_shared<std::vector<bool>>();

    //preset raw values
    std::shared_ptr<std::vector<int>> presetPins = std::make_shared<std::vector<int>>();

    //handles all of the parameter stuff from the hardware
    //raw pin values, change listener, presets etc
    HardwareInterface HWI;
    
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
