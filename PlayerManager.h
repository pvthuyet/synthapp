#pragma once
#include "JuceHeader.h"

struct SynthAudioSource final : public AudioSource
{
    SynthAudioSource (MidiKeyboardState& keyState, const juce::String& sfzPath);
    void loadFile(const juce::String& sfzPath);

    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override;
    void releaseResources() override {}
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;

    // this collects real-time midi messages from the midi input device, and
    // turns them into blocks that we can process in our audio callback
    MidiMessageCollector midiCollector;

    // this represents the state of which keys on our on-screen keyboard are held
    // down. When the mouse is clicked on the keyboard component, this object also
    // generates midi messages for this, which we can pass on to our synth.
    MidiKeyboardState& keyboardState;

    // the synth itself!
    juce::AudioFormatManager formatManager;
    sfzero::Synth synth;
};

//==============================================================================

class Callback final : public AudioIODeviceCallback
{
public:
    Callback (AudioSourcePlayer& playerIn);

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart (AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    AudioSourcePlayer& player;
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