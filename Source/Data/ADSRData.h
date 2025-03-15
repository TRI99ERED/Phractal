/*
  ==============================================================================

    ADSRData.h
    Created: 15 Mar 2025 2:06:53pm
    Author:  tri99er

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class ADSRData : public juce::ADSR {
public:
    void updateADSR(const float attack, const float decay, const float sustain, const float release);
private:
    juce::ADSR::Parameters adsrParams;
};
