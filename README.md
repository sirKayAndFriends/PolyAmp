# PolyAmp
Oh no! Another guitar amp emulator!

This Amp Sim does both preamp and poweramp distortion and includes a choice of two flexible pre-preamp boosts/EQs, a three band tonestack, three channels, and an IR cabsim.

Written in "vanilla" C++ without any special DSP libraries or platform/system code.  This means it can be dropped into pretty much any project that uses C++ (Daisy, Bela, Raspberry Pi, JUCE etc) with only a little tinkering (computer says no).

This repo is the basic source code and will be used as the central documentation!  Included in this README is a full runthrough of the system along with some nice pictures of the architecture.  Maybe there'll be some videos soon too.

There are TWO versions here: PolyAmpSupreme which has all the bells and whistles and the PolyAmpCombo which is meant more as a IRL amplifier and hence has no boost or cabsim.  They otherwise work identically apart from a few modified functions here and there.  The "Documentation" folder uses the Supreme as the example project.

There is NO platform-specific code in this repo!  For examples using the Daisy processor go over [here] and for the VST go over [here].

Before we start - the triode, pentode and ZVA sections were developed by Will Pirkle and come from the addendum to his excellent book *Designing Audio FX Plugins in C++*.  ITS [FREE](https://www.willpirkle.com/fx-book-bonus-material/chapter-19-addendum/)!  Honestly is totally awesome and I learnt a lot - check it out now.

Finally, I am not a programmer. At all.  I am a musician who futzes around with code for fun so I'm sure there are some howlers in here.  I don't care - get a better hobby.  I've kept a lot of it verbose mostly because I wanted to keep it as acessible as possible to beginners and keen opensourcers but also because I'm not a professional.  This is made by amateurs for amateurs ("better than amateurish" - my pal 2025).

