#include "HardwareInterface.h"

void HardwareInterface::setup(double _sampleRate, int _blockSize, std::vector<bool>* _moduleFlags)
{
    sampleRate = _sampleRate;
    dsy_blockSize = _blockSize;
    moduleFlags = _moduleFlags;

    for(int i = 0; i < numParams; i++)
    {
        parameterFlags.push_back(true);
        pinValues.push_back(0.0f);
    }

    listener.setup(numParams);
    listener.setSize(0.025f);

    presetListener.setup(NUM_PRESETS);
    presetListener.setInitial(0);
}

void HardwareInterface::markModuleFlags()
{
    // boostParams
    if(parameterFlags[boostFreq] || parameterFlags[boostAmt] || parameterFlags[boostCut])
    {
        moduleFlags->at(BoostModule) = true;
    }

    // gain
    if (parameterFlags[overdrive])
    {
        moduleFlags->at(PreampModule) = true;
    }

    // tonestack
    if(parameterFlags[toneBass] || parameterFlags[toneMid] || parameterFlags[toneTreble] || parameterFlags[toneVoice])
    {
        moduleFlags->at(TonestackModule) = true;
    }

    // channel
    if(parameterFlags[channel])
    {
        moduleFlags->at(ChannelModule) = true;
    }

    // cabsim
    if(parameterFlags[cabslot])
    {
        moduleFlags->at(CabsimModule) = true;
    }

    // output volume
    if(parameterFlags[volume])
    {
        moduleFlags->at(VolumeModule) = true;
    }
}

void HardwareInterface::parameterCheck(std::vector<float>& knobs, std::vector<bool>& chan, std::vector<bool>& cab)
{
    for(int i = 0; i < NUM_ADC_PARAMS; i++)
    {
        pinValues[i] = knobs[i];
    }

    channelRaw(chan);
    cabSlotsRaw(cab);
    
    listener.query(&pinValues, &parameterFlags);

    //any parameters that are marked "true" are transferred over to processor vector
    for(int i = 0; i < numParams; i++)
    {
        if(parameterFlags[i])
        {
            paramValues[i] = pinValues[i];
        }
    }

    markModuleFlags();
}

bool HardwareInterface::updatePresets(std::vector<int>& presetPins)
{
    bool presetFlag = false;

    presetListener.queryInt(&presetPins, &presetFlags);

    for(int i = 0; i < NUM_PRESETS; i++)
    {
        //if marked for update transfer hardware values over to amp
        if(presetFlags[i])
        {
            //copy preset over to processor values
            if(presetPins[i] > 0)
            {
                assignPreset(i);
            }

            //copy curent hardware values over to preset slot
            else if(presetPins[i] < 0)
            {
                savePreset(i);
            }

            //button toggled off
            //mark all hardware values as needing updated
            else 
            {
                paramValues = pinValues;
                setAllModuleFlags(true);
            }

            presetFlags[i] = false;
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

void HardwareInterface::cabSlotsRaw(std::vector<bool>& cabStates)
{
    auto onIterator = std::find(cabStates.begin(), cabStates.end(), false);

    pinValues[cabslot] = std::distance(cabStates.begin(), onIterator);
}

void HardwareInterface::channelRaw(std::vector<bool>& states)
{
    if (states[0])
    {
        //middle pos - channel 2
        if (states[1])
        {
            pinValues[channel] = 1.0;
        }

        //right pos - channel 3
        else
        {
            pinValues[channel] = 2.0;
        }
    }

    //left pos - channel 1
    else
    {
        pinValues[channel] = 0.0;
    }
}

void HardwareInterface::assignPreset(int index)
{
    if(index == 0)
    {
        for(size_t i = 0; i < paramValues.size(); i++)
        {
            paramValues[i] = preset1[i];
        }
    }

    else if (index == 1)
    {
        for(size_t i = 0; i < paramValues.size(); i++)
        {
            paramValues[i] = preset2[i];
        }
    }

    else if(index == 2)
    {
        for(size_t i = 0; i < paramValues.size(); i++)
        {
            paramValues[i] = preset3[i];
        }
    }

    listener.markAsIgnore(&parameterFlags);
    setAllModuleFlags(true);
}

void HardwareInterface::savePreset(int index)
{
    //parameters saved are processor-side parameters, not hardware
    //only altered values are saved over
    if(index == 0)
    {
        for(size_t i = 0; i < paramValues.size(); i++)
        {
            preset1[i] = paramValues[i];
        }
    }

    else if (index == 1)
    {
        for(size_t i = 0; i < paramValues.size(); i++)
        {
            preset2[i] = paramValues[i];
        }
    }

    else if(index == 2)
    {
        for(size_t i = 0; i < paramValues.size(); i++)
        {
            preset3[i] = paramValues[i];
        }
    }

    listener.markAsIgnore(&parameterFlags);
    setAllModuleFlags(true);
}