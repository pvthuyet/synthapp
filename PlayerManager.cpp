#include "PlayerManager.h"

using namespace juce;

SynthAudioSource::SynthAudioSource (MidiKeyboardState& keyState)
    : keyboardState (keyState)
{
    formatManager.registerBasicFormats();
}

void SynthAudioSource::loadFile(const juce::String sfzPath)
{
    std::cout << sfzPath << std::endl;
    std::cout << "  Loading ...\n";
    std::cout.flush();

    if (!synth.loadSfzFile(sfzPath.toStdString())) {
        std::cerr << "  ====> file loaded failed" << std::endl;
        return;
    }

    std::cout << "  ====> file loaded successfully. " << synth.getNumRegions() << " regions in the SFZ." << std::endl << std::flush;

    sfzFileLoaded = true;
}

void SynthAudioSource::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    midiCollector.reset (sampleRate);
    synth.setSamplesPerBlock(samplesPerBlockExpected);
    synth.setSampleRate((float)sampleRate);
    synth.setNumVoices(128);
    std::cout << "prepareToPlay (samplesPerBlockExpected, sampleRate) = (" << samplesPerBlockExpected << ", " << sampleRate << ")" << std::endl;
}

void SynthAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // the synth always adds its output to the audio buffer, so we have to clear it
    // first..
    bufferToFill.clearActiveBufferRegion();

    // fill a midi buffer with incoming messages from the midi input.
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);

#if DISPLAY_GUI
    // pass these messages to the keyboard state so that it can update the component
    // to show on-screen which keys are being pressed on the physical midi keyboard.
    // This call will also add midi messages to the buffer which were generated by
    // the mouse-clicking on the on-screen keyboard.
    keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
#endif

    // and now get the synth to process the midi events and generate its output.
    if (sfzFileLoaded) {
        // Midi dispatching
        for (auto midi : incomingMidi) {
            auto msg = midi.getMessage();

            if (msg.isNoteOn()) {
                synth.noteOn(0, msg.getNoteNumber(), msg.getVelocity());
            }
            else if (msg.isNoteOff()) {
                synth.noteOff(0, msg.getNoteNumber(), msg.getVelocity());
            }
            else if (msg.isController()) {
                synth.cc(0, msg.getControllerNumber(), msg.getControllerValue());
            }
            else if (msg.isProgramChange()) {
                synth.programChange(0, msg.getProgramChangeNumber());
            }
            else if (msg.isPitchWheel()) {
                synth.pitchWheel(0, msg.getPitchWheelValue());
            }
            else if (msg.isAftertouch()) {
                synth.polyAftertouch(0, msg.getNoteNumber(), msg.getAfterTouchValue());
            }
            else if (msg.isChannelPressure()) {
                synth.channelAftertouch(0, msg.getAfterTouchValue());
            }
        }

        float* stereoOutput[] = { bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getWritePointer(1) };
        synth.renderBlock(stereoOutput, bufferToFill.numSamples);
    }    
}

//==============================================================================

Callback::Callback (AudioSourcePlayer& playerIn)
    : player (playerIn) {}

void Callback::audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                        int numInputChannels,
                                        float* const* outputChannelData,
                                        int numOutputChannels,
                                        int numSamples,
                                        const AudioIODeviceCallbackContext& context)
{
    player.audioDeviceIOCallbackWithContext (inputChannelData,
                                                numInputChannels,
                                                outputChannelData,
                                                numOutputChannels,
                                                numSamples,
                                                context);
}

void Callback::audioDeviceAboutToStart (AudioIODevice* device)
{
    player.audioDeviceAboutToStart (device);
}

void Callback::audioDeviceStopped()
{
    player.audioDeviceStopped();
}

//==============================================================================

PlayerManager::PlayerManager(const juce::String& sfzFile)
    : synthAudioSource(keyboardState),
      callback(audioSourcePlayer)
{
    audioSourcePlayer.setSource (&synthAudioSource);
    
    audioDeviceManager.addAudioCallback (&callback);
    audioDeviceManager.initialise (0, 2, nullptr, true, {}, nullptr);
    audioDeviceManager.addAudioCallback (&callback);

    auto devices = juce::MidiInput::getAvailableDevices();
    if (devices.isEmpty()) {
        std::cout << "There is no Midi keyboard input\n";
        audioDeviceManager.addMidiInputDeviceCallback({}, &(synthAudioSource.midiCollector));
    }
    else {
        for (auto dev : devices) {
            std::cout << "Trying connect to midi input device: " << dev.identifier << std::endl;
            audioDeviceManager.setMidiInputDeviceEnabled(dev.identifier, true);
            audioDeviceManager.addMidiInputDeviceCallback(dev.identifier, &(synthAudioSource.midiCollector));
            std::cout << "  ====> " << dev.identifier << " is " << (audioDeviceManager.isMidiInputDeviceEnabled(dev.identifier) ? "enabled and connect successfully" : "disabled and connect failure") << std::endl;
        }
    }

    std::cout << std::flush;
    juce::Thread::launch([this, sfzFile]() { synthAudioSource.loadFile(sfzFile); });
}

PlayerManager::~PlayerManager()
{
    audioSourcePlayer.setSource (nullptr);

    auto devices = juce::MidiInput::getAvailableDevices();
    if (devices.isEmpty()) {
        audioDeviceManager.removeMidiInputDeviceCallback({}, &(synthAudioSource.midiCollector));
    }
    else {
        for (auto dev : devices) {
            audioDeviceManager.removeMidiInputDeviceCallback(dev.identifier, &(synthAudioSource.midiCollector));
        }
    }

    audioDeviceManager.removeAudioCallback (&callback);
}