#ifndef AMPDEFINITIONS_H
#define AMPDEFINITIONS_H

//comment out these if you don't want a boost or cabsim section
//comment out STEREO if you'd rather have a single output
#define BOOST
#define CABSIM

#define BLOCK_SIZE 24
#define NUM_ADC_PARAMS 9
#define NUM_CHANNELS 3
#define NUM_PRESETS 3
#define NUM_CABS 5

enum moduleNames
{
    BoostModule = 0,
    PreampModule,
    TonestackModule,
    ChannelModule,
    CabsimModule,
    VolumeModule,
    numModules
};

enum parameterNames
{
    boostFreq = 0,
    boostAmt,
    boostCut,
    overdrive,
    toneBass,
    toneMid,
    toneTreble,
    toneVoice,
    volume,
    channel,
    cabslot,

    numParams
};

#endif