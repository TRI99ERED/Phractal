/*
  ==============================================================================

    SynthSound.h
    Created: 14 Mar 2025 10:30:41pm
    Author:  tri99er

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound {
public:
    bool appliesToNote(int midiNoteNumber) override {
        return true;
    }

    bool appliesToChannel(int midiChannel) override {
        return true;
    }
};
