/*
  ==============================================================================

    GlideVisualizer.h
    Author: Aitor

    Muestra una transicion de pitch entre dos notas en funcion del valor
    GLIDE (portamento). A 0 = salto instantaneo, a 2 s = rampa larga.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class GlideVisualizer : public juce::Component, private juce::Timer
{
public:
    GlideVisualizer(juce::AudioProcessorValueTreeState& apvts);
    ~GlideVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& apvtsRef;
    float lastGlide = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlideVisualizer)
};
