#pragma once
#include "JuceHeader.h"

struct SynthAudioSource final : public juce::AudioSource
{
    SynthAudioSource (juce::MidiKeyboardState& keyState);
    void loadFile(const juce::String sfzPath);

    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override;
    void releaseResources() override {}
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    // this collects real-time midi messages from the midi input device, and
    // turns them into blocks that we can process in our audio callback
    juce::MidiMessageCollector midiCollector;

    // this represents the state of which keys on our on-screen keyboard are held
    // down. When the mouse is clicked on the keyboard component, this object also
    // generates midi messages for this, which we can pass on to our synth.
    juce::MidiKeyboardState& keyboardState;

    // the synth itself!
    juce::AudioFormatManager formatManager;
    sfzero::Synth synth;
};

//==============================================================================

class Callback final : public juce::AudioIODeviceCallback
{
public:
    Callback (juce::AudioSourcePlayer& playerIn);

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    juce::AudioSourcePlayer& player;
};

//==============================================================================

class PlayerManager final
{
public:
    PlayerManager(const juce::String& sfzFile);
    ~PlayerManager();

    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }

private:
    juce::AudioDeviceManager audioDeviceManager;
    juce::MidiKeyboardState keyboardState;
    juce::AudioSourcePlayer audioSourcePlayer;
    SynthAudioSource synthAudioSource;
    Callback callback;
};