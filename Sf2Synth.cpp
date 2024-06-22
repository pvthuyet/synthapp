#include "Sf2Synth.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <stdlib.h>
#include <unistd.h>
#endif

Sf2Synth::Sf2Synth()
{}

Sf2Synth::~Sf2Synth()
{
    cleanup();
}

void Sf2Synth::cleanup()
{
    sfontId = FLUID_FAILED;
    if (adriver)
        delete_fluid_audio_driver(adriver);
    if (synth)
        delete_fluid_synth(synth);
    if (settings)
        delete_fluid_settings(settings);
}

bool Sf2Synth::load(const juce::String& sf2Path)
{
    cleanup();

    /* Create the settings. */
    settings = new_fluid_settings();
    if (!settings) {
        puts("Failed to create the settings!");
        return false;
    }

    synth = new_fluid_synth(settings);
    if (!synth) {
        puts("Failed to create the synth!");
        return false;
    }

    std::cout << "  Loading SF2 ...\n";
    auto id = fluid_synth_sfload(synth, sf2Path.toRawUTF8(), 1);
    
    if (id == FLUID_FAILED) {
        puts("Loading the SoundFont failed!");
        return false;
    }

    adriver = new_fluid_audio_driver(settings, synth);
    if(!adriver) {
        puts("Failed to create the audio driver!");
        return false;
    }

    /* Enumeration of banks and programs */
    auto sfont = fluid_synth_get_sfont_by_id(synth, id);
    if (sfont != NULL) {
        fluid_preset_t *preset;
        fluid_sfont_iteration_start(sfont);
        while ((preset = fluid_sfont_iteration_next(sfont)) != NULL) {
            int bank = fluid_preset_get_banknum(preset);
            int prog = fluid_preset_get_num(preset);
            const char* name = fluid_preset_get_name(preset);
			printf("bank: %d prog: %d name: %s\n", bank, prog, name);
		}
	}

    puts("  ====> file loaded successfully.\n");
    sfontId = id;
    return true;
}

void Sf2Synth::playingMidi(const juce::MidiBuffer& incomingMidi)
{
    if (sfontId == FLUID_FAILED)
        return;

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