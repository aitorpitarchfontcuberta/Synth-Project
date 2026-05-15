/*
  ==============================================================================

    LfoVisualizer.h
    Author: Aitor

    Visualizador de LFO con animacion. Refleja:
      - Wave (Sine, Triangle, Saw, Square, S&H)
      - Rate: mas ciclos visibles + mayor velocidad del playhead
      - Depth: amplitud de la onda (0 = linea plana, 1 = onda completa)

    Incluye un "playhead" vertical animado que avanza segun el rate,
    enseniando visualmente como un LFO mas rapido oscila mas veces por segundo.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LfoVisualizer : public juce::Component, private juce::Timer
{
public:
    LfoVisualizer(juce::AudioProcessorValueTreeState& apvts,
                  const juce::String& waveID,
                  const juce::String& rateID,
                  const juce::String& depthID,
                  juce::Colour curveColour);
    ~LfoVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;
    static float renderWave(int waveType, float phase);

    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::String wID, rID, dID;
    juce::Colour colour;

    // Estado de la animacion (interna, no del LFO real)
    float animPhase = 0.0f;
    juce::int64 lastTimeMs = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfoVisualizer)
};
