#include "HardwareInterface.h"

void HardwareInterface::setup
(       
    std::shared_ptr<std::vector<float>> hardwareValues, 
    std::shared_ptr<std::vector<float>> processorValues, 
    std::shared_ptr<std::vector<bool>> _moduleFlags,
    std::shared_ptr<std::vector<int>> _presetPins
)    
{
    hardware = hardwareValues;
    processor = processorValues;
    moduleFlags = _moduleFlags;
    presetPins = _presetPins;

    for(size_t i = 0; i < numParams; i++)
    {
        hardware->push_back(0.0);
        processor->push_back(0.0);
        parameterFlags->push_back(true);
    }

    for(int i = 0; i < numModules; i++)
    {
        moduleFlags->push_back(true);
    }

    for(int i = 0; i < NUM_PRESETS; i++)
    {
        presetPins->push_back(0);
        presetFlags->push_back(false);
    }

    listener.setup(numParams);
    listener.setSize(0.025f);

    presetListener.setup(NUM_PRESETS);
    presetListener.setInitial(0);
}

void HardwareInterface::parameterCheck()
{  
    listener.query(hardware, parameterFlags);

    //any parameters that are marked "true" are transferred over to processor vector
    for(int i = 0; i < numParams; i++)
    {
        if(parameterFlags->at(i))
        {
            processor->at(i) = hardware->at(i);
        }
    }

    debugVal = processor->at(0);

    markModuleFlags();
}

void HardwareInterface::markModuleFlags()
{
    // gain
    if (parameterFlags->at(overdrive))
    {
        moduleFlags->at(PreampModule) = true;
    }

    // tonestack
    if(parameterFlags->at(toneBass) || parameterFlags->at(toneMid) || parameterFlags->at(toneTreble) || parameterFlags->at(toneVoice))
    {
        moduleFlags->at(TonestackModule) = true;
    }

    // channel
    if(parameterFlags->at(channel))
    {
        moduleFlags->at(ChannelModule) = true;
    }

    // output volume
    if(parameterFlags->at(volume))
    {
        moduleFlags->at(VolumeModule) = true;
    }
}

bool HardwareInterface::presetCheck()
{
    bool presetFlag = false;

    presetListener.queryInt(presetPins, presetFlags);

    for(int i = 0; i < NUM_PRESETS; i++)
    {
        //if marked for update transfer hardware values over to amp
        if(presetFlags->at(i))
        {
            //copy preset over to processor values
            if(presetPins->at(i) > 0)
            {
                assignPreset(i);
            }

            //copy curent hardware values over to preset slot
            else if(presetPins->at(i) < 0)
            {
                savePreset(i);
            }

            //button toggled off
            //mark all hardware values as needing updated
            else 
            {
                for(int i = 0; i < numParams; i++)
                {
                    if(parameterFlags->at(i))
                    {
                        processor->at(i) = hardware->at(i);
                    }
                }
                setAllModuleFlags(true);
            }

            presetFlags->at(i) = false;
        }
    }

    return presetFlag;
}

void HardwareInterface::setAllModuleFlags(bool state)
{
    for(size_t i = 0; i < moduleFlags->size(); i++)
    {
        moduleFlags->at(i) = state;
    }
}

void HardwareInterface::assignPreset(int index)
{
    if(index == 0)
    {
        for(size_t i = 0; i < numParams; i++)
        {
            processor->at(i) = preset1[i];
        }
    }

    else if (index == 1)
    {
        for(size_t i = 0; i < numParams; i++)
        {
            processor->at(i) = preset2[i];
        }
    }

    else if(index == 2)
    {
        for(size_t i = 0; i < numParams; i++)
        {
            processor->at(i) = preset3[i];
        }
    }

    listener.markAsIgnore(parameterFlags);
    setAllModuleFlags(true);
}

void HardwareInterface::savePreset(int index)
{
    //parameters saved are processor-side parameters, not hardware
    //only altered values are saved over
    if(index == 0)
    {
        for(size_t i = 0; i < numParams; i++)
        {
            preset1[i] = processor->at(i);
        }
    }

    else if (index == 1)
    {
        for(size_t i = 0; i < numParams; i++)
        {
            preset2[i] = processor->at(i);
        }
    }

    else if(index == 2)
    {
        for(size_t i = 0; i < numParams; i++)
        {
            preset3[i] = processor->at(i);
        }
    }

    listener.markAsIgnore(parameterFlags);
    setAllModuleFlags(true);
}