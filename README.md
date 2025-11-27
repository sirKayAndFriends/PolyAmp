# PolyAmp
Oh no! Another guitar amp emulator!

This valve amp emulator is written in "vanilla" C++ without any special DSP libraries or platform/system code.  This means it can be dropped into pretty much any project that uses C++ (Daisy, Bela, Raspberry Pi, JUCE etc) with only a little tinkering (computer says no).

This repo is the basic source code and will be used as the central documentation!  Included in this README is a full runthrough of the system along with some nice pictures of the architecture.  Maybe there'll be some videos soon too.

Before we start - the triode, pentode and ZVA sections were developed by Will Pirkle and come from the addendum to his excellent book *Designing Audio FX Plugins in C++*.  ITS [FREE](https://www.willpirkle.com/fx-book-bonus-material/chapter-19-addendum/)!  Honestly is totally awesome and I learnt a lot - check it out now.

Finally, I am not a programmer. At all.  I am a musician who futzes around with code for fun so I'm sure there are some howlers in here.  I don't care - get a better hobby.  I've kept a lot of it verbose mostly because I wanted to keep it as acessible as possible to beginners and keen opensourcers but also because I'm not a professional.  This is made by amateurs for amateurs ("better than amateurish" - my pal 2025).

# OVERVIEW
## BOOST
A boost section designed to cover most of the types of boosts guitarists like.  This is a boost-only high-shelf filter with a variable frequency ("A high-shelf filter passes all frequencies, but increases or reduces frequencies above the shelf frequency by specified amount" [wikipedia](https://en.wikipedia.org/wiki/Filter_design)).  The frequency goes from 700hz to 2500hz to cover everything from Tubescreamer-ish sounds to Rangemaster-esque noises.  There is also a boost/cut low-shelf filter set at 100hz - useful cutting bass from mid/treble boosts or boosting bass like a Stringray!  

This module can be easily omitted by commenting out "#define BOOST 1" in core/ampDefinitions.h

*Knobs: boostAmt, boostFreq, boostCut*

## PREAMP
The top level of the preamp module - handles channel switching and parameter scaling.  Included in this source code are three channel objects ("loChannel", "midChannel", "hiChannel") which are dreived from the "channel" base class.  Make your own and override like a C++ master!

LO CHANNEL is sort of like an early fender amp.  Three triodes, very little bass filtering, two-stage gain control and a modest gain boost at the start.  Nice cleans and goes a bit ratty if pushed.

MID CHANNEL jcm800 ish? Four triodes with more focussed bass filtering and more pre-gain and inter-triode gain.

HI CHANNEL 90s "high gain" I think.  Four triodes, severe filtering and lots more gain.

*Knobs: overdrive, (channel)*

## POWER AMP
Two pentodes for more distortion.  Not a lot of control over these guys, set-and-forget the input gain and saturation with the channel control.

*Knobs: (channel)*

## TONE STACK
A three band active EQ section with parametric mid control.  Bass and treble are constant frequency (150hz and 2000hz respectively) with the control being a cut/boost gain control.  The mid section is a peaking filter with a gain control and frequency control which sweeps from 400hz to 850hz.  This is the "voice" of the amp and is used to get those nice american or british sounds

*Knobs: toneBass, toneMid, toneTreble, toneVoice*

## CABSIM
An FFT convolver for guitar cab Impulse Responses.  Load in your fave guitar cabs!  (Check out the guide for adding IRs at the bottom).  This uses the [HiFi-LoFI FFT convolver](https://github.com/HiFi-LoFi/FFTConvolver) which is super simple to use and works very, very nicely.  Legend. (I made my own FIR filter and it was bad - it used about 80% of the Daisy's CPU with only a 128 sample IR).  Five slots in total but fewer can be used with no code editing (see guide) - one of the slots can also be reserved as a bypass.

This module can be easily omitted by commenting out "#define CABSIM 1" in core/ampDefinitions.h

*Knobs: cabslot*

## CHANNEL
This isn't really a module and has mostly been covered but I thought I'd cover it here anyway.  The channel switch updates the preamp, poweramp and output modules all at once.  The preamp switches to a different object for processing each with their own triodes, parameters and settings (lo, mid, or hi); the poweramp simply has two variables updated (input gain "sag" and saturation); the output has three volume variables, one for each channel.  See the next section for info.

*Knobs: channel*

## OTHER MODULES
also included are an Output module (just a block-based volume control with separate max values for each channel) and two antialiasing lowpass filters (set at 10,000hz and placed after the preamp and poweramp modules).  Pretty simple.  

The output is maybe over-engineered but volume control is a little tricky.  Internally there can be over 20x gain which needs to be scaled down again - but the "mid" and "hi" channels also are clipped severely which lowers the volume ("compression").  So in some cases the "lo" channel is waaay louder than the others as the volume increase is more linear.  Ho hum.  Also the cabsim adds a fair amount of volume depending on the IR.  Also you don't want to clip the output of whatever microcontroller you're using OR the op-amp buffers after the output.  So yeah I did my best and tried to make the clean sounds as loud as possible while still keeping the volume knob as usable as possible.  Feel free to tinker.

By default the program has a stereo output but can be modded to be a mono output by commenting out the line "#define STEREO 1" in core/ampDefinitions.h

# Architecture
So this section is a decription of how the whole thing functions.  I had three design principles: keep it as readable as possible for beginners; keep it as consistent as possible; keep it as moddable as possible.  

These principles in most cases trumped performance and memory (programmers log off) so sometimes the workflow is a bit OTT in order to keep the general working consistent.

## HARDWARE LEVEL
The very top level will be your hardware program and, as such, is not included here.  This will be the Daisy or Bela program or whatever that contains the main while loop and the audiocallback (or whatever).  Keep the amp setup to the main function, the parameter reading and updating to the while loop and the audio processing to the audio callback.  I have a Daisy specific repo coming soon!

## AMP LEVEL
This is the PolyAmp class and is the program's top level found in amp.cpp.  This owns all the modules and handles parameter listening, module updating, preset listening and audio processing (shuttling the buffer between each module).  Linked to this are all the files in "core", most importantly HardwareInterface which receives the raw values from the microprocessor.  This class folds the different data types (float or bool for knob or switch) into a single array which is then checked against the Listener.  This will report if any values have changed - this step filters out ADC noise, stops the program updating EVERYTHING every cycle (particularly problematic with the cabsim), and allows us to load presets and have the program ignore the knobs (unless they are changed!).  The HardwareInterface groups the "parameter flags" into "module flags" - for example if any of the boost controls have changed then the whole boost module is marked as needing updated.  PolyAmp then steps back in and updates the requisite module.

## MODULE LEVEL
Modules are: boost, preamp, poweramp, tonestack, cabsim and output.  The channel control covers preamp, poweramp and output.  This level mirrors the topology of a valve amplifer and organises all the components under them to keep it all nice and tidy.  The module classes deal with parameter scaling (up until this point all values have been 0.0 - 1.0 or true/false) and contain most of the variables of interest for modders.  For example the *Tonestack* class contains variables such as the min and max values for the mid control sweep as well as the filter class components.  The module level also controls which part of the module get updated and when.

## COMPONENT LEVEL
The very bottom.  These are the filters, triodes, pentodes and convolvers that do the actual DSP processing.  These have their own variables but are less interesting for modders and will have less effect on the tone of the amp.  They are all bundled together to form a coherent assemblage by the module level

## in summary...
The Hardware Level controls all your lovely knobs and switches and passes them to...  
The Amp Level which organises the flow of data, checks for updates and passes raw hardware data to each module in...  
The Module Level which holds all the "tone" information like clipping levels and tonestack values for...  
The Component Level which does the actual DSPing - analogous to a single triode or whatevs  


