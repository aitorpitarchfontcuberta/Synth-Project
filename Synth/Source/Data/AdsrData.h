/*
  ==============================================================================

    AdsrData.h
    Created: 30 Apr 2026 11:36:41am
    Author:  Aitor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AdsrData : public juce::ADSR
{
public:
    void updateADSR(const float attack, const float decay, const float sustain, const float release);


private:
    juce::ADSR::Parameters adsrParams;
};