# PolyAmp
Oh no! Another guitar amp emulator!

This valve amp emulator is written in "vanilla" C++ without any special DSP libraries or platform/system code.  This means it can be dropped into pretty much any project that uses C++ (Daisy, Bela, Raspberry Pi, JUCE etc) with only a little tinkering (computer says no).

This repo is the basic source code and will be used as the central documentation!  Included in this README is a full runthrough of the system along with some nice pictures of the architecture.  Maybe there'll be some videos soon too.

Before we start - the triode, pentode and ZVA sections were developed by Will Pirkle and come from the addendum to his excellent book *Designing Audio FX Plugins in C++*.  ITS [FREE](https://www.willpirkle.com/fx-book-bonus-material/chapter-19-addendum/)!  Honestly is totally awesome and I learnt a lot - check it out now.

Finally, I am not a programmer. At all.  I am a musician who futzes around with code for fun so I'm sure there are some howlers in here.  I don't care - get a better hobby.  I've kept a lot of it verbose mostly because I wanted to keep it as acessible as possible to beginners and keen opensourcers but also because I'm not a professional.  This is made by amateurs for amateurs ("better than amateurish" - my pal 2025).

# OVERVIEW
## BOOST
A boost section designed to cover most of the types of boosts guitarists like.  This is a boost-only high-shelf filter with a variable frequency ("A high-shelf filter passes all frequencies, but increases or reduces frequencies above the shelf frequency by specified amount" [wikipedia](https://en.wikipedia.org/wiki/Filter_design)).  The frequency goes from 700hz to 2500hz to cover everything from Tubescreamer-ish sounds to Rangemaster-esque noises.  There is also a boost/cut low-shelf filter set at 100hz - useful cutting bass from mid/treble boosts or boosting bass like a Stringray!  
*Knobs: boostAmt, boostFreq, boostCut*

## PREAMP
The top level of the preamp module - handles channel switching and parameter scaling.  Included in this source code are three channel objects ("loChannel", "midChannel", "hiChannel") which are dreived from the "channel" base class.  Make your own and override like a C++ master!

LO CHANNEL is sort of like an early fender amp.  Three triodes, very little bass filtering, two-stage gain control and a modest gain boost at the start.  Nice cleans and goes a bit ratty if pushed.

MID CHANNEL jcm800 ish? Four triodes with more focussed bass filtering and more pre-gain and inter-triode gain.

HI CHANNEL 90s "high gain" I think.  Four triodes, severe filtering and lots more gain.

*Knobs: overdrive, (channel)*

