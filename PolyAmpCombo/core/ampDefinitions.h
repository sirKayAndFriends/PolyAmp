#ifndef AMPDEFINITIONS_H
#define AMPDEFINITIONS_H

#define BLOCK_SIZE 24
#define NUM_ADC_PARAMS 6
#define NUM_CHANNELS 3
#define NUM_PRESETS 3

enum moduleNames
{
    PreampModule = 0,
    TonestackModule,
    ChannelModule,
    VolumeModule,
    numModules
};

enum parameterNames
{
    overdrive = 0,
    toneBass,
    toneMid,
    toneTreble,
    toneVoice,
    volume,
    channel,

    numParams
};

#endif