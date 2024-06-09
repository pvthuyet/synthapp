#include "MainComponent.h"
#include "PlayerManager.h"

//==============================================================================
MainComponent::MainComponent(PlayerManager& player)
    : mPlayer(player),
      keyboardComponent(player.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible (keyboardComponent);
    keyboardComponent.setWantsKeyboardFocus(true);
    
    setOpaque (true);
    setSize (640, 68);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    keyboardComponent.setBounds (8, 2, getWidth() - 16, 64);
}