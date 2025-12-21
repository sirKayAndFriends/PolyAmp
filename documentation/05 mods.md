## MODDING

I have anticipated lots of tweaking and modding of this project so I have tried to make it as easy as possible.  I have grouped these into the following categories:

1) individual parameter mods
2) module mods
3) code mods
4) To Dos
5) Project Inspiration!

### Individual Parameter Mods
This is the easiest to mess around with.  Most of the values you'll want to mess with are in the "Module Level" in tonestack.h or powerAmp.h.  The preamp module has some too but also check out the channel files!

Basically this boils down to changing numbers here and there.  Have fun!

I have created a channel base class from which to derive your own channels if you want.  You'll need to add triode objects and pass them audio samples in whatever way you please.  Refer to my own hardcoded examples or mess with them - its all good.

If you want to remove single hardware parameters (like "treble" or whatever) all you need to do is supply a static value.  This is covered in the howto but I'll do an example here too.  Lets say you want a project with a boost that has a fixed frequency of 700hz and no bass control.  You want all three channels but no voice control as you just want a Marshall sound. No Presets.

your while look would include the following (remember the Amp object wants values of 0.0 - 1.0 so you'll have to do some maths - but I believe in you!):

amp.updateParameters(
    pin1.Read(),            //boostGain
    0.0f,                   //boostFrequency
    0.5f,                   //boostBass
    pin2.Read(),            //distortion
    pin3.Read(),            //treble        
    pin4.Read(),            //mid
    pin5.Read(),            //bass    
    1.0f                    //voice   
    pin6.Read(),            //masterVolume
    channelNum,             //channel
    cabslotNum,             //cabslot
    0,                      //preset1
    0,                      //preset2
    0                       //preset3
);

Remember, if you want to cut down on knobs just send the amp a static value!

### Module Mods
Slightly trickier but not by much.  I anticipate that the modules wanting removed by users are the boost and cabsim modules.  I have included these pre-processor directives in core/ampDefinitions.h:

#define BOOST
#define CABSIM

If you go and look in amp.cpp you'll see the boost and cabsim processBlock() functions enclosed by an #ifdef.  This is some clever C++ stuff that makes me feel like a pro.  Basically if the pre-processor sees that CABSIM has been defined (as above) then it includes the cabsim #ifdef in compilations.  If you comment out "#define CABSIM" then the pre-processor will NOT include it!  To be clear, this is different from encapsulating the the cabsim processBlock() in an if statement: this will stay in the code and the program will have to evaluate it every time (and cost processing time).  This method precedes compiling - its like the block of code never existed!  If you're using a snazzy IDE then it will even grey those blocks out.

You will notice that this is the only part encapsulated.  All the other processing happens (parameters, updating etc) but the actual audio processing will be missed so this is definitely a fast and dirty way of doing it.  BUT it is fast and, because our MPU is dedicated to this amp, we don't need to be super fussy about processing power.  

If you want to get rid of the boost and/or cabsim but don't have a lot of code experience just comment out or delete those #define lines.

BE CAREFUL.  You will still need to use the updateParameters() function as before with the total list of arguments (they'll be ignored).  If you don't then you'll have knobs changing unexpected parameters and maybe some vector subscript failures.

Alternatively you can go through the code and remove all references.  This is more time consuming and may throw some errors that beginners may find confusing.  I tend to let the IDE do the hard work for me and just delete the files that I don't want then go through the folders looking for the red squiggly lines (or build the project and have the compiler shout at me).  I assume the people wanting to do this are competent so you don't need me lecturing you.

### Code Mods
Experts go at it.  

Be aware of core/ampDefinitions.h though.  I have included those enums and #defines to make reading the code easier for beginners.  It does make some of the program static however - especially the use of the enums.  Make sure you've taken those into account.

The variable definitions are not super important to change and are really only upper limits used to resize the parameter vectors on startup.  They also make the links between memory and paramters more clear.  Unless you are doing substantial changes you can just ignore them.

### To Dos
I built this program for the Daisy so was somewhat limited by processing power (only one thread) and GPIO (I have a bunch of fancy RGB LEDs too).  There are definitely more things that could be added.

I think a lot of people would like a treble control for the boost.  I wasn't fussed and didn't want the extra knob but a fixed frequency high shelf filter might be cool.  Should be easy to add with the provided classes too.

Dedicated channel switching stomps.  I think the preset buttons cover this (and it lets you set the individual volumes too) but that may be a cool addition.

Better antialiasing?  I'm pretty sure I've got this down but I'm not an expert and it could maybe do with improving.

Output volume.  Again not an expert.  The analog circuits I used for the Daisy are the ones Electrosmith provide diagrams for (in the Petal etc) and the buffers have a 5v power supply and biased to 2.5v.  This is lower than your usual guitar pedal and its there to protect the Daisy's inputs.  I had some trouble on the output though as the op-amps started to clip waaay earlier than I was expecting.  I had to balance volume levels across various distortion sounds AND make sure the outputs didn't clip.  Tricky.

The obvious solution to this is to have a 9v output buffer.  BUT you don't your audio power mixing with your Daisy power and there's already a digital 9v and isolated audio 5v.  Adding an isolated 9v power for the output seems excessive.

### Inspiration
Here are some cool ideas for modded projects!

- make an amplifier! remove boost and cabsim (and voice?).  Plug into some sort of power amp (one of those cheap class A chips perhaps) and into a speaker cabinet.  You could remove the channel switch (and hardcode the channel you want) or add drive and volume controls for each channel.
- make drive pedal.  Remove boost, cabsim and tonestack (maybe keep the treble) and just have a "valve emulator" effect
- make this effect as standard but put in a headphone amp or bluetooth thing for home practice
- put in LEDS! I have two RGB LEDS controlled by software PWM - they change colour when I move the voice or boostFreq knobs
- LCD display maybe.  I personally hate these.