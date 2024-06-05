#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(const juce::String& sfzFile)
    : synthAudioSource(keyboardState, sfzFile),
      callback(audioSourcePlayer)
{
    audioSourcePlayer.setSource (&synthAudioSource);
    audioDeviceManager.addAudioCallback (&callback);
    audioDeviceManager.initialise (0, 2, nullptr, true, {}, nullptr);
    audioDeviceManager.addAudioCallback (&callback);

    auto devs = juce::MidiInput::getAvailableDevices();

    for (auto d : devs) {
        DBG(d.name << ", " << d.identifier);
        audioDeviceManager.setMidiInputDeviceEnabled(d.name, true);
        audioDeviceManager.addMidiInputDeviceCallback(d.identifier, &(synthAudioSource.midiCollector));
    }

    setOpaque(true);
    setSize(400, 250);
}

MainComponent::~MainComponent()
{
    audioSourcePlayer.setSource (nullptr);
    audioDeviceManager.removeMidiInputDeviceCallback ({}, &(synthAudioSource.midiCollector));
    audioDeviceManager.removeAudioCallback (&callback);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}