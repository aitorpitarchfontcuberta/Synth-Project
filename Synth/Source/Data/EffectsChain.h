/*
  ==============================================================================

    EffectsChain.h
    Author: Aitor

    Cadena de efectos globales aplicada despues del synth.
    Orden: Chorus -> Delay -> Reverb (el orden mas natural para que la reverb
    suavice todo).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class EffectsChain
{
public:
    void prepareToPlay(double sampleRate, int blockSize, int numChannels);
    void reset();

    void process(juce::AudioBuffer<float>& buffer);

    // --- CHORUS ---
    void setChorusEnabled(bool enabled)     { chorusOn = enabled; }
    void setChorusParams(float rate, float depth, float feedback, float mix);

    // --- DELAY ---
    void setDelayEnabled(bool enabled)      { delayOn = enabled; }
    void setDelayParams(float timeMs, float feedback, float mix);

    // --- REVERB ---
    void setReverbEnabled(bool enabled)     { reverbOn = enabled; }
    void setReverbParams(float size, float damping, float width, float mix);

private:
    double sampleRate = 44100.0;
    int    blockSize = 512;

    // CHORUS
    juce::dsp::Chorus<float> chorus;
    bool  chorusOn = false;

    // DELAY (estereo, una linea por canal)
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayL { 96000 };
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayR { 96000 };
    bool  delayOn = false;
    float delayTimeSamples = 0.0f;
    float delayFeedback    = 0.0f;
    float delayMix         = 0.0f;
    juce::SmoothedValue<float> delayTimeSmoothed;
    juce::SmoothedValue<float> delayMixSmoothed;
    juce::SmoothedValue<float> delayFbSmoothed;

    // REVERB
    juce::dsp::Reverb reverb;
    bool  reverbOn = false;
    float reverbMix = 0.0f;
    juce::dsp::Reverb::Parameters reverbParams;
};
