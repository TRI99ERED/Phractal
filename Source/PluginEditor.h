/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/ADSRComponent.h"
#include "UI/OscComponent.h"

//==============================================================================
/**
*/
class PhractalAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PhractalAudioProcessorEditor (PhractalAudioProcessor&);
    ~PhractalAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PhractalAudioProcessor& audioProcessor;
    
    OscComponent osc;
    ADSRComponent adsr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhractalAudioProcessorEditor)
};
