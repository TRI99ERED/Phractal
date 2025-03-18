/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PhractalAudioProcessorEditor::PhractalAudioProcessorEditor (PhractalAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), fr(audioProcessor), osc(audioProcessor.apvts, "OSCWAVETYPE"), adsr(audioProcessor.apvts)
{
    setSize(1280, 720);

    addAndMakeVisible(fr);
    addAndMakeVisible(osc);
    addAndMakeVisible(adsr);
}

PhractalAudioProcessorEditor::~PhractalAudioProcessorEditor()
{
}

//==============================================================================
void PhractalAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void PhractalAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    osc.setBounds(bounds.removeFromLeft(280));
    fr.setBounds(bounds.removeFromTop(500));
    adsr.setBounds(bounds);
}
