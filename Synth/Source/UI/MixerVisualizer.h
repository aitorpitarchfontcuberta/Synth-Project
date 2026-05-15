/*
  ==============================================================================

    MixerVisualizer.h
    Author: Aitor

    Visualiza la forma de onda resultante del Mixer (combinacion OSC1 + OSC2
    segun el modo, mas Sub) reproduciendo internamente la misma logica que
    el motor para mostrar exactamente lo que sale antes del envelope.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MixerVisualizer : public juce::Component, private juce::Timer
{
public:
    MixerVisualizer(juce::AudioProcessorValueTreeState& apvts);
    ~MixerVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    // Genera una muestra de una onda con la fase dada (phase en [0,1))
    static float renderWave(int waveType, float phase, float pw);

    juce::AudioProcessorValueTreeState& apvtsRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerVisualizer)
};
