#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(const juce::String& sfzFile)
    : synthAudioSource(keyboardState, sfzFile),
      keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
      callback(audioSourcePlayer)
{
    addAndMakeVisible (keyboardComponent);
    keyboardComponent.setWantsKeyboardFocus(true);
    audioSourcePlayer.setSource (&synthAudioSource);
    
    audioDeviceManager.addAudioCallback (&callback);
    audioDeviceManager.initialise (0, 2, nullptr, true, {}, nullptr);
    audioDeviceManager.addAudioCallback (&callback);
    audioDeviceManager.addMidiInputDeviceCallback ({}, &(synthAudioSource.midiCollector));

    setOpaque (true);

    setSize (640, 480);
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

void MainComponent::resized()
{
    keyboardComponent.setBounds (8, 96, getWidth() - 16, 64);
}