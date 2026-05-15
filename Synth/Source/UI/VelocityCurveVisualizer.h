/*
  ==============================================================================

    VelocityCurveVisualizer.h
    Author: Aitor

    Dibuja la curva de respuesta a velocity MIDI segun el parametro VEL_SENS.
    Formula: amp = (1 - velSens) + velSens * velocity, exactamente la misma
    que utiliza el motor en SynthVoice.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class VelocityCurveVisualizer : public juce::Component, private juce::Timer
{
public:
    VelocityCurveVisualizer(juce::AudioProcessorValueTreeState& apvts);
    ~VelocityCurveVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& apvtsRef;
    float lastVelSens = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityCurveVisualizer)
};
