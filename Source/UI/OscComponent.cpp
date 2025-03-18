/*
  ==============================================================================

    OscComponent.cpp
    Created: 15 Mar 2025 3:03:30pm
    Author:  tri99er

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscComponent.h"

//==============================================================================
OscComponent::OscComponent(juce::AudioProcessorValueTreeState& apvts, juce::String waveSelectorId)
{
    juce::StringArray choices { "Mandelbrot", "Burning ship", "Feather", "SFX", "Henon", "Duffing", "Ikeda", "Chirikov" };

    oscWaveSelector.addItemList(choices, 1);

    addAndMakeVisible(oscWaveSelector);

    oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts,
        waveSelectorId,
        oscWaveSelector
    );
}

OscComponent::~OscComponent()
{
}

void OscComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void OscComponent::resized()
{
    auto bounds = getLocalBounds();
    oscWaveSelector.setBounds(bounds.removeFromTop(30).reduced(5));
}
