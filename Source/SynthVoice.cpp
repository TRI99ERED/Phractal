/*
  ==============================================================================

    SynthVoice.cpp
    Created: 14 Mar 2025 10:29:42pm
    Author:  tri99er

  ==============================================================================
*/

#include "SynthVoice.h"

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {
    osc1.setWaveFrequency(midiNoteNumber);
    osc2.setWaveFrequency(midiNoteNumber);
    adsr.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff) {
    adsr.noteOff();

    if (!allowTailOff || !adsr.isActive()) {
        clearCurrentNote();
    }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue) {

}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue) {

}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels) {
    adsr.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    osc1.prepareToPlay(spec);
    osc2.prepareToPlay(spec);
    gain.prepare(spec);

    gain.setGainLinear(0.3f);

    isPrepared = true;
}

void SynthVoice::update(const float attack, const float decay, const float sustain, const float release) {
    adsr.updateADSR(attack, decay, sustain, release);
}

void SynthVoice::renderNextBlock(juce::AudioBuffer< float >& outputBuffer, int startSample, int numSamples) {
    jassert(isPrepared);

    if (!isVoiceActive()) {
        return;
    }

    leftSynthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, false);
    leftSynthBuffer.clear();

    juce::dsp::AudioBlock<float> leftAudioBlock { leftSynthBuffer };

    osc1.getNextAudioBlock(leftAudioBlock);
    gain.process(juce::dsp::ProcessContextReplacing<float>(leftAudioBlock));

    adsr.applyEnvelopeToBuffer(leftSynthBuffer, 0, leftSynthBuffer.getNumSamples());

    outputBuffer.addFrom(0, startSample, leftSynthBuffer, 0, 0, numSamples);

    rightSynthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, false);
    rightSynthBuffer.clear();

    juce::dsp::AudioBlock<float> rightAudioBlock{ rightSynthBuffer };

    osc2.getNextAudioBlock(rightAudioBlock);
    gain.process(juce::dsp::ProcessContextReplacing<float>(rightAudioBlock));

    adsr.applyEnvelopeToBuffer(rightSynthBuffer, 0, rightSynthBuffer.getNumSamples());
    
    outputBuffer.addFrom(1, startSample, rightSynthBuffer, 1, 0, numSamples);

    if (!adsr.isActive()) {
        clearCurrentNote();
    }
}

