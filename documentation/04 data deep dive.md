## Data and Structures
This section is going to be about how data and parameters are passed around.  Its more complicated than the DSP.

I will start off by documenting how the overall hardware and preset management works and then move onto the data architecture for the rest of the project.

### Naming Conventions
In core/ampDefinitions.h there are a couple of enums that are used to make the code more readable.  My coding style uses lots of vectors and therefore lots of indices!  I personally find this:

params[overdrive]

easier to read than this:

params[3]

There are lots of parameters and modules banging around and the order in which vectors are filled is important so I generally find this kind of naming less buggy.

core/ampDefinitions.h is also where you will find important global variables like NUM_PARAMS and such

## Amp Parameters
As mentioned in the howto you need to update your amp object in the main while loop with the following function:

amp.updateParameters(
    float boostGain, float boostFrequency, float boostBassCut,
    float distortion,
    float treble, float mid, float bass, float voice,
    float masterVolume,
    int channel,
    int cabslot,
    int preset1, int preset2, int preset3
);

the float values are simple - they need to be between 0.0 and 1.0 like all ADC pin values.

The int values are a bit different.  In this case I opted for "simple to understand" rather than "simple to code" (often mutually exclusive in C++).  We'll leave the presets for later and focus on channel and cabslot here.

Both channel and cabslot expect an int representing the "slot" to use.  0 = channel 1, 1 =  channel 2 etc.
This is not how GPIOs work however (button pins are *true* or *false*) so you will need to write some code of your own to convert the various pin values into a single int.  This goes against one of my principles: as little coding as possible for the user.  In this case this principle is trumped by "as moddable/flexible as possible".  I found that any other method (an array of bools, for example, representing each pin value) involved doing some internal logic that anticipated the kind of hardware used and I felt this undermined the flexibility.

For example, I originally intended the channel switch to be a DPDT On-Off-On.  This is commonly available and allows three positions.  The three states are {1 0} {0 0} {0 1} with the middle position disconnecting both lugs.  But then I had to write a bunch of code that converts those three logic states into three outcomes which excluded some hardware options.  I ended up trying to write a load of different functions and setups to accomodate various choices and I eventually realised that I should just forget it.  This document is already long enough.

So to reiterate - updateParameters() wants a single int value representing which channel or cab you want.  It is up to you how your hardware works with this!  

### Internal Data Management
Remember those levels from the Architecture doc?  Those will come in handy now.

The Hardware level only understands the ADC and GPIO data.  The Amp level sorts these into modules and works out when to update and such.  It still only sees "raw" ADC/GPIO data.

The module level is where the scaling magic happens that turns our raw data into musical info.

Each module has a struct of variables that the Amp object needs access to.  These are named ModuleNameData (boostData, preampData etc).  The Amp grabs a shared pointer to these structs on startup and updates them whenever instructed to.  When the Amp has finished transferring the results it calls update on that module.  The module takes over, scales all of those new values according to its own private variables and sorts out where they go.

Some of the modules have lots of accessible variables (looking at you tonestack) so the struct sharing method seemed the neatest.  In order to keep everything consistent I have done this with every module (even if they only have like two variables).

The preamp module has two update routes.  The overdrive knob changes the preamp's input gain variable and the channel switch changes which channel is being processed.

### Hardware Interface
This project is mainly intended to be used with embedded systems - "chips" - like Daisy or Bela.  The main "problem" here is that all of the hardware values are read continuously.  If we follow this route then we end up updating all of the modules every time the hardware samples the pin voltages.  This is annoying because we don't necessarily need to update the amplifier this frequently (like when we've got the amp set up just right and can just leave it be while we play).  We get into further mess when we want to add presets (see the later section)

So, this amplifier uses a "listener" topology which adds a fair amount of complexity to the project.

It goes like this.  The hardware level reads pin values continuously and sends them to the amp object.  Amp checks collates all the values into a single vector, *pinValues* and checks with Hardware Interface (HWI).  The HWI compares this vector to the one stored from the previous cycle.  If anything has changed HWI takes note of that specific parameter and copies that parameter value from *pinValues* into another vector called *paramValues*.  As well as this HWI groups these changes into modules.  These flags are stored in a vector called "moduleFlags" which is owned by the amp object.  HWI then stores the newly recieved parameter values to check them against the *next* cycle.  This is the end of HWI's process loop.

Having checked with HWI the amp looks through the *moduleFlags* vector for any *true* flags and updates the modules accordingly.

For example:

I move the boost frequency knob from 0.0 to 0.5.  This is immediately sent to the amp in the updateParameters() function.  Amp groups this and all the other hardware values into *pinValues* and sends it to be checked for changes by HWI.  HWI compares *pinValues* with the previous, saved version of same.  It replies that yes, that specific knob has recently been twiddled.  In switches the appropriate index in *parameterFlags* to *true* and saves all the values for the next time round.

HWI then runs through *parameterFlags* looking for *true* flags.  If it finds any (our boost frequency value in this case) it copies it over to *paramValues* and ignores any pin value that has stayed the same.  Next it looks to see which module this parameter belongs to (Boost in this case) and logs the appropriate index in *moduleFlags* as *true*.  HWI can now rest easy, its job is done.

On completion of this task the amp object is in possession of a newly updated *moduleFlags* vector and *paramValues* vector.  Amp checks for *true* values in *moduleFlags* and if it finds any updates the module's parameters with values from *paramValues* and then calls the module update function.

Visually it looks like this:

pinValues =      {0.0,    0.5,  0.0,   0.0};        // write only
                   x       |     x      x
                   x       |     x      x
parameterFlags = {false, true, false, false};       // "gate"
                   x       |     x      x
                   x       V     x      x
paramValues =    {0.0,    0.0,  0.0,   0.0};        // read only

The *parameterFlags* and *moduleFlags* vectors act as gates to the flow of data.  We can turn them all to false which stops data being passed - ignore the hardware! - and stops the modules from being updated.  This removes unnecessary calculations being done (like filters being update with the same values over and over again) but also allows us to have an internal set of parameters that does not match the actual position of the knobs - which is what we want for our preset manager.

### Presets
The preset functionality by design has to be split into two sections.  The first is the internal working of the parameters and the second is the system-dependent way in which preset data is stored in non-volatile memory.  This second obviously will be different for every piece of hardware so I won't cover it here.

When updating the hardware values in amp.updateParameters() you will notice the last three parameters are for preset values.  The program expects one of three values ONLY for each preset pin: 

1 - preset on (load preset and proceed)
-1 - save current values as preset
0 - turn preset off and return to reading hardware

I will leave the hardware programming up to you.  I like to have two presets controlled by two momentary stomp switches.  I have some code that toggles a value every time it is pressed - "software latching" rather than "mechanical latching".  This covers the 0 and 1 values.  Holding the switch down is the -1 (which is why i use momentary switches - you can't hold down a latching one) and on release sets the value to 1 (so if you save a preset it then turns on preset mode).

If you want to use presets then you will also need to call amp.attachPresetStorage() and pass pointers to vectors of parameter values.  Check out my Daisy example project for help - here is some of that:

float factory1[numParams] =
{
    1.0,    //2500hz frequency     |  
    0.5,    //50% boost            | - BOOST
    0.5,    //flat bass         ___|
    0.5,    //50% overdrive     ___| - GAIN
    0.5,    //flat bass            |  
    0.7,    //slight mid bump      | - TONESTACK
    0.5,    //flat treble          |
    1.0,    //850hz mid frequency__|   
    0.5,    //50% volume           | - VOLUME
    1.0,    //mid gain channel     | - CHANNEL
    2.0     //cab slot 2           | - CAB
};

amp.attachPresets(factory1, factory2, factory3);

(note float values and order of parameters)

The amp object collates the hardware values read from the pins attached to the preset buttons in your project.  This vector *presetPins* is checked with HWI for any changes in a similar way to the way in which parameters are checked.  If there has been a change (i.e. a preset button has been pressed) the HWI logs that change in the vector *presetFlags*.  This is the same length as *presetPins* and the indices correlate.  Preset button #1's values are stored in *presetPins[0]* and if this value changes the *presetFlags[0]* changes to *true*.  HWI then loops through *presetFlags* looking for *true* values.  If one is found one of three things may happen.

A "1" will trigger the assignPreset() function which will copy the values from the one of the preset vectors (according the the index of *presetFlags*) to *paramValues*.  This function will also mark all of the *parameterFlags* as false and all of the *moduleFlags* as true.  

The first operation blocks any transfer of *pinValues* over to *paramValues* - if you think about it the preset values will be different from the current pin values so we don't want to immediately overwrite them.  These values are not saved in the Listener's history but I'm not sure if this matters as moving any knob will immediately trigger an update anyway.

The second operation instructs all the modules to update as they will have received new preset data but will not know it as the Listener has been instructed to ignore changes in parameters so will not have logged this change.

A "-1" will call the savePresets() function.  The current *paramValues* are copied into the appropriate preset vector (and is then saved to the systems non-volatile memory).  In my system (where a -1 is followed by a 1) this then triggers assignPreset().

A 0 tells HWI to start reading hardware values again  *pinValues* is copied to *paramValues* and all modules are instructed to update.

I know this is complicated and the workflow is serpentine.  Because the source of the data comes from two places (hardware or memory) we need to be able to control when we read from hardware and when we update modules independently.

Imagine this scenario on a simplified project which only contains drive, volume and treble and bass.  I have my hardware set at:

drive - 0.5
volume - 0.75
treble - 0.75
bass - 0.5

I then load a preset that changes the amp to:

drive - 1.0
volume - 0.25
treble - 0.75
bass - 0.5

I want the amp to stay in this second state right? Not revert immediately to the first.  This is why I have kept the place that the hardware data is stored and the place the amp reads its parameter info from totally separate and to only copy over when there has been a change.  

Say I then think that this preset has too much treble so move the treble knob to 0.5.  The HWI notes this change and copies ONLY THAT VALUE over into *paramValues* keeping the drive, volume and bass values the same:

drive - 1.0
volume - 0.25
treble - 0.5
bass - 0.5

When saving a preset it is *paramValues* that is saved.  If, like in this example, you load a preset that is totally different from the current hardware positions, tweak it then save it to the same slot you've only saved the changes (the treble knob) and not the hardware values.

If you don't want to do any of this just send 0s to the preset parameters in updateParameters() and the program will ignore them :))