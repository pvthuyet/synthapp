#include "SfzSynth.h"

SfzSynth::SfzSynth()
{}

SfzSynth::~SfzSynth()
{}

bool SfzSynth::load(const juce::String& sfzPath)
{
    bool success = false;
    std::cout << "  Loading SFZ ...\n";
    if (synth.loadSfzFile(sfzPath.toStdString())) {
        std::cout << "  ====> file loaded successfully. " << synth.getNumRegions() << " SFZ regions\n";
        success = true;
    }
    else {
        std::cout << "  ====> file loaded failed.\n";
    }

    std::cout << std::flush;
    return success;
}

void SfzSynth::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    synth.setSamplesPerBlock(samplesPerBlockExpected);
    synth.setSampleRate((float)sampleRate);
    synth.setNumVoices(128);
}

void SfzSynth::playingMidi(const juce::MidiBuffer& incomingMidi)
{
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
}

void SfzSynth::renderBlock(float* leftBuffer, float* rightBuffer, int numFrames)
{
    float* stereoOutput[] = { leftBuffer, rightBuffer };
    synth.renderBlock(stereoOutput, numFrames);
}