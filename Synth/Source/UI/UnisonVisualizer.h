/*
  ==============================================================================

    UnisonVisualizer.h
    Author: Aitor

    Representa visualmente las voces de unison apiladas, con su detune.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class UnisonVisualizer : public juce::Component, private juce::Timer
{
public:
    UnisonVisualizer(juce::AudioProcessorValueTreeState& apvts);
    ~UnisonVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& apvtsRef;
    int lastVoices = -1;
    float lastDetune = -1.0f;
    float lastSpread = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnisonVisualizer)
};
