#pragma once
#include "JuceHeader.h"
#include "fluidsynth.h"
#include <atomic>

class Sf2Synth final
{
private:
    fluid_settings_t* settings = nullptr;
    fluid_synth_t* synth = nullptr;
    fluid_audio_driver_t *adriver = nullptr;
    std::atomic_int sfontId = FLUID_FAILED;

public:
    Sf2Synth();
    ~Sf2Synth();

    bool load(const juce::String& sf2Path);
    void playingMidi(const juce::MidiBuffer& incomingMidi);

private:
    void cleanup();
};