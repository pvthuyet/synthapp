#pragma once
#include "JuceHeader.h"
#include "fluidlite.h"

class FluidSynth final
{
private:
    fluid_settings_t* settings = nullptr;
     fluid_synth_t* synth = nullptr;

public:
    FluidSynth();
    ~FluidSynth();

    bool load(const juce::String& sfPath);
    void playingMidi(const juce::MidiBuffer& incomingMidi);
    void renderBlock(float* leftBuffer, float* rightBuffer, size_t numFrames);
};