#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include "JuceHeader.h"

struct SynthAudioSource final : public AudioSource
{
    SynthAudioSource (MidiKeyboardState& keyState, const juce::String& sfzPath)
        : keyboardState (keyState)
    {
        formatManager.registerBasicFormats();
        for (int i = 0; i < 128; ++i) {
            synth.addVoice(new sfzero::Voice());
        }

        loadFile(sfzPath);
    }

    void loadFile(const juce::String& sfzPath)
    {
        DBG("Loading sf2|sfz");
        synth.clearSounds();
        auto sfzFile = File(sfzPath);

        double loadProgress = 0;
        synth.clearSounds();

        if (!sfzFile.existsAsFile()) {
            return;
        }

        std::unique_ptr<sfzero::Sound> sound = nullptr;
        auto extension = sfzFile.getFileExtension();
        if ((extension == ".sf2") || (extension == ".SF2")) {
            sound = std::make_unique<sfzero::SF2Sound>(sfzFile);
        }
        else {
            sound = std::make_unique<sfzero::Sound>(sfzFile);
        }
        sound->loadRegions();
        sound->loadSamples(&formatManager, &loadProgress, nullptr);
        synth.addSound(sound.release());

        DBG("file loaded!");
    }

    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        midiCollector.reset (sampleRate);
        synth.setCurrentPlaybackSampleRate (sampleRate);
    }

    void releaseResources() override {}

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // the synth always adds its output to the audio buffer, so we have to clear it
        // first..
        bufferToFill.clearActiveBufferRegion();

        // fill a midi buffer with incoming messages from the midi input.
        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);

        // pass these messages to the keyboard state so that it can update the component
        // to show on-screen which keys are being pressed on the physical midi keyboard.
        // This call will also add midi messages to the buffer which were generated by
        // the mouse-clicking on the on-screen keyboard.
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);

        // and now get the synth to process the midi events and generate its output.
        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    }

    //==============================================================================
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


class Callback final : public AudioIODeviceCallback
{
public:
    Callback (AudioSourcePlayer& playerIn)
        : player (playerIn) {}

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override
    {
        player.audioDeviceIOCallbackWithContext (inputChannelData,
                                                 numInputChannels,
                                                 outputChannelData,
                                                 numOutputChannels,
                                                 numSamples,
                                                 context);
    }

    void audioDeviceAboutToStart (AudioIODevice* device) override
    {
        player.audioDeviceAboutToStart (device);
    }

    void audioDeviceStopped() override
    {
        player.audioDeviceStopped();
    }

private:
    AudioSourcePlayer& player;
};


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent final : public juce::Component
{
public:
    //==============================================================================
    MainComponent(const juce::String& sfzFile);
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================

    juce::AudioDeviceManager audioDeviceManager;
    juce::MidiKeyboardState keyboardState;
    juce::AudioSourcePlayer audioSourcePlayer;
    SynthAudioSource synthAudioSource;
    MidiKeyboardComponent keyboardComponent;
    Callback callback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
