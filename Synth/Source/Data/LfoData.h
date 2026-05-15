/*
  ==============================================================================

    LfoData.h
    Created: 2026
    Author:  Aitor

    LFO simple basado en fase. Soporta Sine, Triangle, Saw, Square y S&H.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LfoData
{
public:
    enum WaveType { Sine = 0, Triangle, Saw, Square, SampleHold };

    void prepareToPlay(double newSampleRate);
    void resetPhase();

    void setWaveType(int choice);
    void setRateHz(float rateHz);

    // Genera la siguiente muestra del LFO, en el rango [-1, 1]
    float renderSample();

private:
    double sampleRate = 44100.0;
    double phase = 0.0;
    double phaseIncrement = 0.0;

    int   waveType = Sine;
    float rate     = 1.0f;

    float shValue = 0.0f;
    juce::Random random;
};
