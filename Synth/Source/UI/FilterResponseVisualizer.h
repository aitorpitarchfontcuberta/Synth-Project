/*
  ==============================================================================

    FilterResponseVisualizer.h
    Author: Aitor

    Dibuja la respuesta en frecuencia del filtro (magnitud vs frecuencia,
    eje X log de 20 Hz a 20 kHz, eje Y en dB). Reacciona en tiempo real
    al tipo de filtro, cutoff y resonance.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Data/ScopeBuffer.h"

class FilterResponseVisualizer : public juce::Component, private juce::Timer
{
public:
    FilterResponseVisualizer(juce::AudioProcessorValueTreeState& apvts,
                             const ScopeBuffer& scopeBuffer);
    ~FilterResponseVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& apvtsRef;
    const ScopeBuffer& bufferRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterResponseVisualizer)
};
