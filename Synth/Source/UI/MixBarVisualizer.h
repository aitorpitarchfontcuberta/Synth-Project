/*
  ==============================================================================

    MixBarVisualizer.h
    Author: Aitor

    Barra horizontal que muestra el balance entre OSC1 y OSC2.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MixBarVisualizer : public juce::Component, private juce::Timer
{
public:
    MixBarVisualizer(juce::AudioProcessorValueTreeState& apvts,
                     const juce::String& mixParamID);
    ~MixBarVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::String mixID;
    float lastMix = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixBarVisualizer)
};
