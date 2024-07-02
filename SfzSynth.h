#pragma once
#include "JuceHeader.h"
#include "sfizz.hpp"

class SfzSynth final
{
private:
    sfz::Sfizz synth;

public:
    SfzSynth();
    ~SfzSynth();

    bool load(const juce::String& sfzPath);

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void playingMidi(const juce::MidiBuffer& incomingMidi);
    void renderBlock(float* leftBuffer, float* rightBuffer, int numFrames);
};