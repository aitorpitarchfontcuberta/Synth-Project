/*
  ==============================================================================

    SpectrumComponent.h
    Author: Aitor

    Visualizador en dominio frecuencial. Lee del ScopeBuffer compartido,
    aplica una ventana Hann y calcula una FFT. Muestra el espectro de
    magnitud con eje X logaritmico (20 Hz - 20 kHz) y eje Y en dB.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Data/ScopeBuffer.h"

class SpectrumComponent : public juce::Component, private juce::Timer
{
public:
    SpectrumComponent(const ScopeBuffer& buffer,
                      juce::AudioProcessorValueTreeState* apvtsForFilterOverlay = nullptr);
    ~SpectrumComponent() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    const ScopeBuffer& bufferRef;
    juce::AudioProcessorValueTreeState* apvtsRef = nullptr;

    static constexpr int kFftOrder = 11;            // 2^11 = 2048
    static constexpr int kFftSize  = 1 << kFftOrder;

    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, (size_t)(kFftSize * 2)> fftData {}; // entrada + zero padding
    std::array<float, (size_t) kFftSize>      magnitudes {}; // magnitudes en lineal
    std::array<float, (size_t) kFftSize>      magsSmoothed {}; // smoothing visual

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumComponent)
};
