/*
  ==============================================================================

    WaveformVisualizer.h
    Author: Aitor

    Dibuja la forma de onda seleccionada (sine, triangle, saw, square, pulse,
    noise) leyendo del APVTS. Para "Pulse" tambien refleja el pulse width.

    Si se pasan IDs de octave/semi/fine, el numero de ciclos visibles cambia
    segun el pitch relativo (educativo: el usuario ve mas ciclos al subir
    octava o al detune positivo).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WaveformVisualizer : public juce::Component, private juce::Timer
{
public:
    enum class Mode { Oscillator, Lfo };

    WaveformVisualizer(juce::AudioProcessorValueTreeState& apvts,
                       const juce::String& waveID,
                       const juce::String& pulseWidthID,
                       juce::Colour curveColour,
                       Mode mode = Mode::Oscillator,
                       const juce::String& octaveID = juce::String(),
                       const juce::String& semiID   = juce::String(),
                       const juce::String& fineID   = juce::String());
    ~WaveformVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::String wID, pwID;
    juce::String octID, semID, finID;
    juce::Colour colour;
    Mode mode;

    int   lastWave   = -1;
    float lastPW     = -1.0f;
    int   lastOctave = -99;
    int   lastSemi   = -99;
    float lastFine   = -999.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformVisualizer)
};
