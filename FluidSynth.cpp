#include "FluidSynth.h"

FluidSynth::FluidSynth()
{
    settings = new_fluid_settings();
    synth = new_fluid_synth(settings);
}

FluidSynth::~FluidSynth()
{
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}

bool FluidSynth::load(const juce::String& sfPath)
{
    auto id = fluid_synth_sfload(synth, sfPath.toRawUTF8(), 1);
    return id != -1;
}

void FluidSynth::playingMidi(const juce::MidiBuffer& incomingMidi)
{
    for (auto midi : incomingMidi) {
        auto msg = midi.getMessage();

        if (msg.isNoteOn()) {
            fluid_synth_noteon(synth, msg.getChannel(), msg.getNoteNumber(), msg.getVelocity());
        }
        else if (msg.isNoteOff()) {
            fluid_synth_noteoff(synth, msg.getChannel(), msg.getNoteNumber());
        }
        else if (msg.isController()) {
            fluid_synth_cc(synth, msg.getChannel(), msg.getControllerNumber(), msg.getControllerValue());
        }
        else if (msg.isProgramChange()) {
            fluid_synth_program_change(synth, msg.getChannel(), msg.getProgramChangeNumber());
        }
        else if (msg.isPitchWheel()) {
            fluid_synth_pitch_bend(synth, msg.getChannel(), msg.getPitchWheelValue());

        }
        else if (msg.isAftertouch()) {
            fluid_synth_key_pressure(synth, msg.getChannel(), msg.getNoteNumber(), msg.getAfterTouchValue());
        }
        else if (msg.isChannelPressure()) {
            fluid_synth_channel_pressure(synth, msg.getChannel(), msg.getAfterTouchValue());
        }
    }
}

void FluidSynth::renderBlock(float* leftBuffer, float* rightBuffer, size_t numFrames)
{
    fluid_synth_write_float(synth, numFrames, leftBuffer, 0, 2, rightBuffer, 1, 2);
}