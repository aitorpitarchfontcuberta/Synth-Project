/*
  ==============================================================================

    OscilloscopeComponent.h
    Author: Aitor

    Osciloscopio en tiempo real. Lee del ScopeBuffer compartido y dibuja
    la senial de salida. Implementa "trigger" simple por cruce por cero
    ascendente para que la traza no tiemble.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Data/ScopeBuffer.h"

class OscilloscopeComponent : public juce::Component, private juce::Timer
{
public:
    OscilloscopeComponent(const ScopeBuffer& buffer);
    ~OscilloscopeComponent() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    const ScopeBuffer& bufferRef;

    static constexpr int kDrawSamples = 2048; // muestras que se dibujan
    std::array<float, kDrawSamples> drawData {};
    float peakLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscilloscopeComponent)
};
