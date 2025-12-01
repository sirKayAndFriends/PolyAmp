## How to use PolyAmp

create a new amplifier object:

PolyAmp amp;

The amp object then needs to be initialised:

amp.setup(samplerate, blocksize);

these two parameters will be available in whatever platform you are using

in the main while loop of the program you will need to pass the hardware values on to the amp object with the updateParameters() function:

amp.updateParameters(
    float boostGain, float boostFrequency, float boostBassCut,
    float distortion,
    float treble, float mid, float bass, float voice,
    float masterVolume,
    int channel,
    int cabslot,
    int preset
);

- the float values are all expected to be 0.0 - 1.0
- channel, cabslot and preset are integers starting at 0.

I tried a few different methods for exposing the amplifier variables and settled on this way.  Remembering my design principles I wanted this program to be as easy to use for beginners as possible - a single named function with all of the parameter values laid out seemed way simpler than updating vectors or creating shared pointers etc.

This method also allows for the simplest possible modding.  If you want fewer pieces of hardware you can just send a static value to the amplifier and go and have a cup of tea.  The example below shows a 5 knob project with boostGain, distortion, treble, mid and volume controls and static values for boost frequency, bass cut, bass, voice, channel, cabsim and preset.  (This would be marshall style amplifer on medium gain channel with a treble boost, no cabsim, no presets).

amp.updateParameters(
    pin1.Read(),
    1.0f,
    0.5f,
    pin2.Read(),
    pin3.Read(),
    0.5f,
    1.0f,
    pin4.Read(),
    pin5.Read(),
    1,
    0,
    0
);

## Audio
In the audio callback function put the following code:

amp.processBlock(inputBuffer, outputBuffer, blockSize);

The amplifier processes by block rather than by sample so you need to pass the system's audio buffer to be filled along with the buffer's size.  With the Daisy the input buffer is constant and you are expected to copy the values out of it and into whatever processing you are doing, then copy those results into the output buffer.  I have overloaded the processBlock() function to allow for systems that only supply a single buffer that you are expected to overwrite.

Now comes the delicate part.  Daisy supplies its buffers, in a very C-like way, as pointers-to-floats.  This pointer points to the first value of an array from which the computer can get the rest of the array's data.  This is fairly common in C as passing an array by default actually only passes a pointer to the first value:

float buffer[128];

void processBlock(float* input, int size)
{
    for(int i = 0; i < size; i++)
    {
        input[i] = 1.0f;
    }
}

//we're not actually passing the literal array here, just telling the computer where to find the array in memory.
processBlock(buffer, 128);

This is smart because you're only passing a small value rather than the entire array of audio samples.  However, in my humble opinion, it reads like total garbage.  This is what the input buffer looks like in Daisy:

const float* const* input;

Visually the input looks like this:

input = 
{
    {0.1, 0.2, 0.3, 0.4},
    {0.1, 0.2, 0.3, 0.4}
};

Its a stereo buffer! An array of arrays if you will. Breaking down the data type of input is a bit complex - its a *constant pointer* to a *pointer to a constant float*.  Basically we're pointing the computer to the memory address of that first 0.1 *which is constant and cannot be changed* (so the pointer also has to constant hence the const*).

The output data type is a little simpler:

float** output;

this is a pointer to a pointer to a float in normal C-speak but in reality this is how you write a pointer to an array of arrays which we musicians know better as a stereo buffer.  The output data type is therefore a pointer to the first value of the first channel of the output buffer.

So in summary: when you see float** this means that the data is going to be stereo:

output =        <--- first pointer points to here
{
      v------------------- second pointer points to here!
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0}
}

That ridiculous input data type is just the weird C way of making it constant (so you can't get it all sticky and untidy).

This is all to say that when using processBlock() you need to work out how to pass the correct audio buffer.  Most audio processors will give you stereo buffers so you need to get a pointer to the channel that holds your input guitar samples - usually this is channel 0.  

(PolyAmp uses mono buffers for input and output - copy over the amplifiers output to the other channel if you want stereo.  This is *technically* stereo but both channels are identical.  I built PolyAmp on the Daisy which did not have enough processing power for a more interesting stereo effect (like stereo cabsims or whatnot) so I stuck to simple mono processing).

void AudioCallback(AudioHandle::InputBuffer  in, AudioHandle::OutputBuffer out, size_t size)
{    
    //create a constant pointer to the first sample of the first channel (float) of the INPUT BUFFER
    const float* input = &in[0][0]; 

    //create a pointer to the first sample of the first channel (float) of the OUTPUT BUFFER
    float* output = &out[0][0];

    ampProcessor.processBlock(input, output, size);
}

or for systems where you're just supplied with a single buffer you can use the overloaded function:

void AudioCallback(InputBuffer audioBuffer, size_t size)
{    
    //create a pointer to the first sample of the first channel (float) of the OUTPUT BUFFER
    float* buffer = &audioBuffer[0][0];

    //in JUCE you can do this
    float* buffer = audioBuffer.getWritePointer(0);   //0 = first channel

    ampProcessor.processBlock(buffer, size);
}