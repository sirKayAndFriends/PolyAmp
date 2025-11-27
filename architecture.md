## Architecture
So this section is a decription of how the whole thing functions.  I had three design principles: keep it as readable as possible for beginners; keep it as consistent as possible; keep it as moddable as possible.  

These principles in most cases trumped performance and memory (programmers log off) so sometimes the workflow is a bit OTT in order to keep the general working consistent.

### HARDWARE LEVEL
The very top level will be your hardware program and, as such, is not included here.  This will be the Daisy or Bela program or whatever that contains the main while loop and the audiocallback (or whatever).  Keep the amp setup to the main function, the parameter reading and updating to the while loop and the audio processing to the audio callback.  I have a Daisy specific repo coming soon!

### AMP LEVEL
This is the PolyAmp class and is the program's top level found in amp.cpp.  This owns all the modules and handles parameter listening, module updating, preset listening and audio processing (shuttling the buffer between each module).  Linked to this are all the files in "core", most importantly HardwareInterface which receives the raw values from the microprocessor.  This class folds the different data types (float or bool for knob or switch) into a single array which is then checked against the Listener.  This will report if any values have changed - this step filters out ADC noise, stops the program updating EVERYTHING every cycle (particularly problematic with the cabsim), and allows us to load presets and have the program ignore the knobs (unless they are changed!).  The HardwareInterface groups the "parameter flags" into "module flags" - for example if any of the boost controls have changed then the whole boost module is marked as needing updated.  PolyAmp then steps back in and updates the requisite module.

### MODULE LEVEL
Modules are: boost, preamp, poweramp, tonestack, cabsim and output.  The channel control covers preamp, poweramp and output.  This level mirrors the topology of a valve amplifer and organises all the components under them to keep it all nice and tidy.  The module classes deal with parameter scaling (up until this point all values have been 0.0 - 1.0 or true/false) and contain most of the variables of interest for modders.  For example the *Tonestack* class contains variables such as the min and max values for the mid control sweep as well as the filter class components.  The module level also controls which part of the module get updated and when.

### COMPONENT LEVEL
The very bottom.  These are the filters, triodes, pentodes and convolvers that do the actual DSP processing.  These have their own variables but are less interesting for modders and will have less effect on the tone of the amp.  They are all bundled together to form a coherent assemblage by the module level

### in summary...
The Hardware Level controls all your lovely knobs and switches and passes them to...  
The Amp Level which organises the flow of data, checks for updates and passes raw hardware data to each module in...  
The Module Level which holds all the "tone" information like clipping levels and tonestack values for...  
The Component Level which does the actual DSPing - analogous to a single triode or whatevs  

