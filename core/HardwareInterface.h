#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include "listener.h"
#include "ampDefinitions.h"

class HardwareInterface
{
public:

    double sampleRate;

    int dsy_blockSize;

    /*IMPORTANT VOCAB!!!
    "processor values" = the data actually being read by the amp processor
    updated when marked as changed by the listener
    not yet scaled
    found in the vector "paramValues"

    "hardware values" = the data coming in from the knobs/buttons etc
    updated at the while loop rate in main()
    this is "write only" so to speak
    found in vector "pinValues"

    the workflow here:

    - daisy sends over pin values, channel values, cab values continuously
    - ampParameters normalises these (floats and bools) into float values in one big vector
    - listener is queried per parameter slot to see if any values have changed - true/false answers stored in "updateFlags"
    - if true copy value over to processor values
    - markModuleFlags() called
        - updateFlags are grouped into modules in moduleFlags (owned by processor)
        - processor uses these to update individual modules


    Preset handling working concurrently:

    - daisy sends over preset button info continuously (see presetHW.h / .cpp)
    - if a preset slot has been marked 1:
        - the appropriate preset array is loaded into paramValues
        - updateFlags is marked as false stopping hardware values being copied over immediately
        - all module flags are marked as true so the processor knows to update!
    - if a preset slot has been marked -1:
        - cue for saving a preset slot
        - paramValues are read into preset slot
        - on button release the slot is marked 1 - go back to slot marked as 1!
        - note that processor (NOT hardware) values are saved.  If, after loading a preset, you've only altered one value
        (treble for example) then this is the only change that will be saved.  This prevents unforeseen changes
        when hardware values are at odds with processor ones
    - if a preset is marked 0:
        - preset listener is checked - if no change then nothing happens
        - if there has been a change this means the button has been toggled off - updateFlags is marked as true and 
        hardware values are read into param values.  this turns off preset mode and is the default 
        mode if you're not using preset buttons
 
    Keeping the pin and param values separate allows for presets to be loaded as we can tell ampParameters to ignore incoming hardware values
    by keeping the copying / actions separate from one another
    we can control what is ignored or copied just by changing the slots in updateFlags

    */

    void setup(
        std::shared_ptr<std::vector<float>> hardwareValues, 
        std::shared_ptr<std::vector<float>> processorValues, 
        std::shared_ptr<std::vector<bool>> updateFlags,
        std::shared_ptr<std::vector<int>> presetPins
    );
    
    //check parameter values
    void parameterCheck();
    
    bool presetCheck();

    //pointers to preset vectors stored in the Daisy external flash memory
    //these are passed from the "daisy level" main function
    float* preset1;
    float* preset2;
    float* preset3;

    float debugVal;

private:

    //parameter listener
    //fires whenever a knob moves
    Listener listener;
    Listener presetListener;

    //individual parameter markers
    std::shared_ptr<std::vector<bool>> parameterFlags = std::make_shared<std::vector<bool>>();
    
    //same for preset parameters
    std::shared_ptr<std::vector<bool>> presetFlags = std::make_shared<std::vector<bool>>();

    //vector from processor
    std::shared_ptr<std::vector<bool>> moduleFlags;
    std::shared_ptr<std::vector<float>> hardware;
    std::shared_ptr<std::vector<float>> processor;
    std::shared_ptr<std::vector<int>> presetPins;

    //runs through individual parameters and groups them into modules
    //e.g. if any tone control is changed (treble, bass etc)
    //then the whole tonestack is updated
    void markModuleFlags();

    //set all the module flags to one value
    //means the amp processor can update the modules 
    //without reading from hardware values
    void setAllModuleFlags(bool state);

    //change to a new setting
    void assignPreset(int index);

    //save the current processor values in this slot
    void savePreset(int index);
};
