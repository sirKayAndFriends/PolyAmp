## NERD SECTION

ok this part will go through some of the actual DSP.  Check out the "Architecture" section for a simple overview of the workflow.

This project started as an attempt to make a pocket sized amplifier for musicians with accessibility issues (like me) who can't necessarily heft a marshall stack (or don't know any muscly people).  I then opened my laptop and it GOT OUT OF HAND.

I own one of those Tech-21 sansamp pedals that emulates a Vox AC30.  It is very nice so I went on the internet to see how it was made and came across [this](https://www.freestompboxes.org/viewtopic.php?t=6571&sid=4639cef9970ee09f2878ac72e0959d8a) thread on freestompboxes which totally blew my mind.  It was very fun to read.

I was surprised to learn how much of the tone comes from the tone controls (I guess I'd never said that out loud) and very little from the overdrive.  In fact, the overdrive section was the same in every box!

Each different "character" has a boost section and tone section that corresponds to the "voice" of the amp they are emulating (fender, marshall etc) and there's a cab sim too. 

This was the original template for my design which is why there is a boost with variable voice, an active tonestack with variable mid frequency, an overdrive (with power amp!) and a cabsim.

so here we go...

### global components

This folder has components used by multiple modules.  They are Pirkle's ZVA-style filter which is used by the triode and pentode classes to calculate the DC offset.  Gonna be honest, I don't particularly understand the maths here but it works and sounds different than when you don't use it so here we are.  I have simplified how it works quite a bit and just kept the low-pass element which is the only one used.

The second component is skFilters which is a biquad filter made from:
//https://www.gcradix.org/post/introduction-the-biquad-filter
Thanks!

This is the standard filter topology used for the entire project (exception being the FIR filter) and can be created as a lowpass, highpass, highshelf, lowshelf, or peak.  Pretty rad.  The style is decided in the class constructor using an enum for the type.  The calc() method sets up all the complex variables according to the filter type.  process() and processBlock() filter audio by sample or block respectively.  

This IR filter design is used as they are quick and easy to recalculate - which we'll be doing a lot!

### Boost
The boost section is made out of three separate filters: a lowshelf (LS), a highshelf (HS), and a lowpass (LP).  The LP cuts a whole tone of treble from the incoming guitar signal and is never altered - this is both for tone and for antialiasing.  5000hz seemed about right but go ahead and change that if you want.

The LS is the bass cut - although its actually both cut and a boost.  The frequency is static at 100hz and is useful for such boosty sounds as a tubescreamer or rangemaster where there is significant bass reduction.  It can also work like the Stringray bass preamp!

The HS is the boost and has a variable frequency of 700hz to 2500hz to cover all types of boost/overdrive pedals (although there's no clipping here).  I originally used peaking filters for the boost but it didn't sound very guitary and the more I thought about it the more I realised that all our favourite boosts are actually high pass filters with a volume boost.  So shelving filters felt much better.  There is no upper frequency control in this model so if you're picky about EQ you could add in a treble shelf (like the tubescreamer) but I opted for having one less knob.

### Preamp
This one is a complex one.  I'll defer to Will Pirkle (see link in README) to explain the workings of the emulation and stick to describing the channel stuff.

The preamp module organises the collection of channels, scales all their variables and handles the channel switching.  Each channel is its own class (derived from the channel.h base class) with its own hard-coded variables and collection of triode objects.  Feel free to twiddle with mine or inherit your own and go nuts.

I chose this route as I initially had just the one set of triode objects but changing all the parameters for each channel change (not always the same ones) alongside of keepin track of how many triodes each channel was actually using was a pain and super-prone to bugs. In the bin it went.  This way means everything is tucked away neatly in its own file that you can go in and fiddle with if you need.  The preamp class stores pointers to the channels in a vector which means changing a channel is as simple as:

std::vector<Channel*> channels;
int activeChannel;

channels[activeChannel]->processBlock(buffer, size);

no if statement needed.

This means that updating is super simple as we only need to change the gain variable and the channel number.  In previous tries I was sending in like 10 different values.

Inside the channel objects.  The setup function initialises all of the channel variables.  This is really the place you want to focus on for modding as it has the most tone options:

gainMultiplier - the amount the gain knob (0.0 - 1.0 remember) is multiplied by.  I have a gain control after the first and second triodes like a dualgang pot in some amps.

outputGain (triode) - the triode's output gain multiplier.  Very useful for setting the style of the preamp.

waveSaturation (triode) - one of the key variables for the amount of distortion within each triode.  I like 0.5 for low gain, 1.0 for mid, and 2.0 for high.

outputLSFreq / outputHSFreq - the triode's filtering.  This sets the voicing of the preamp in a similar way to the boost section.  This is really important for getting the character of the sound - more aggressive bass filtering will get that tight high gainy thing.  The higher the gain gets without proper bass control the mushier the distortion will be as each triode object starts to overload so be careful.

Each triode also has a 7000hz lowpass filter to help with antialiasing.

In the Daisy environment I found 4 triodes to be the limit - any more and with all the filtering and FIRing going on it will start to choke.

### Power Amp
This is a pretty static module.  Check out Pirkle's book again for the physics.  The pentode objects are not dissimilar to the triode class just with a different distortion algorithm.

The important variables here are the saturation and sag (input gain really).  They work in tandem to set the distortion level and are changed when the channel switch is changed.  Low gain: saturation 1.2, sag 1.0; High gain: saturation 2.0, sag 2.0;

### Tonestack
Three biquad filters.  Lowshelf and highshelf with variable gain and static frequencies and a peaking filter with variable gain *and* frequency.  Bass is set at 250hz, treble at 2000hz. Gain values are set to +/- 12.0.
The mid control is the real hero.  The variable frequency runs from 400hz to 850hz which can cover a whole range of amp tones.  This is not "accurate" to a real guitar amplifier as there is no contouring or anything like that.  BUT it does allow you to get all sorts of sounds and is super versatile and easy to use.  Boosting the mids gets you tweedy or marshally sounds and cutting can get you fendery or rectifiery sounds.

### Cabsim
A fast FFT convolver courtesy of HiFi-LoFi (check out README for links)!  Very easy to use and way, WAY better than my first attempt.  Does some very nice cab IR filtering thank you very much.

I have defined the IR size at the top of cabism_fft.h thusly:

#define IR_SIZE 1024

This I think is good enough for everyone and a higher value runs the risk of overloading your system.

The cabsim updater runs through a vector of pointers to impulse responses (see next paragraph) to load into the convolver.  If the vector index is "nullptr" then nothing is loaded and this is interpreted to mean that the cabsim is in bypass mode.

Getting the IRs in place is another story.  Basically, there is no space on the Daisy to store wav files and to do that kind of processing so we'll be doing it before flashing.  I have written a python script which is included in this repo that can be run before flashing the cabs and works as follows.  Put your IR wav files in the folder called "impulseResponses".  Its a good idea to change the file names to include a number at the start ("01_G12.wav" for example) otherwise the order will get mixed up but otherwise filenames do not matter.  Run the python script before building the project - the script will scan the folder, load the wav files, write the first 1024 samples of the wav into a buffer then create a header file called cabIRs.h in the cabsim folder.  This file will contain the IR information saved in vectors along with a vector of pointers to the IRs, a numCabs variable and a variable that indicates which swithc position will be the "off" position.  This is by defualt 0 but can be altered by changing the argument in the python script.

As stated in the "howto" you can bypass the cabsim from your project by commenting out the CABSIM definition.  However, flashing a built project with no cabIRs file will also be fine.  The processor will just ignore it.


### Other Modules
There are two other modules which are useful but not super exciting.  

The two antialias modules are simple lowpass filters set at 10,000hz.  Stops antialiasing.  The default Daisy samplerate is 48khz so we don't want any samples above 24khz.  Luckily for us, from a tone point of view, we want to start sloping the tone off around 8000hz and aggressively filtering above 10,000hz.

The output is maybe over-engineered but volume control is a little tricky. Internally there can be over 20x gain which needs to be scaled down again - but the "mid" and "hi" channels also are clipped severely which lowers the volume ("compression"). So in some cases the "lo" channel is waaay louder than the others as the volume increase is more linear. Ho hum. Also the cabsim adds a fair amount of volume depending on the IR. Also you don't want to clip the output of whatever microcontroller you're using OR the op-amp buffers after the output. So yeah I did my best and tried to make the clean sounds as loud as possible while still keeping the volume knob as usable as possible. Feel free to tinker.

