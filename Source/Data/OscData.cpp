/*
  ==============================================================================

    OscData.cpp
    Created: 15 Mar 2025 2:39:26pm
    Author:  tri99er

  ==============================================================================
*/

#include "OscData.h"

void OscData::prepareToPlay(juce::dsp::ProcessSpec& spec) {
	prepare(spec);
}

void OscData::setWaveType(const int choice) {
	switch (choice)
	{
	case 0:
		// Sine wave
		initialise([](float x) { return std::sin(x); });
		break;
	case 1:
		// Saw wave
		initialise([](float x) { return x / juce::MathConstants<float>::pi; });
		break;
	case 2:
		// Square wave
		initialise([](float x) { return x < 0.f ? -1.f : 1.f; });
		break;
	default:
		jassertfalse;
		break;
	}
}

void OscData::setWaveFrequency(const int midiNoteNumber) {
	setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
}

void OscData::getNextAudioBlock(juce::dsp::AudioBlock<float>& block) {
	process(juce::dsp::ProcessContextReplacing<float>(block));
}
