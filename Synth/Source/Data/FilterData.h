/*
  ==============================================================================

    FilterData.h
    Author: Aitor

    Filtro IIR por voz. Soporta LP, HP, BP y Notch con cutoff y resonance.
    Incluye un drive opcional (saturacion tanh suave) antes del filtro.

    Tambien expone una funcion estatica para calcular la respuesta en
    magnitud a una frecuencia dada, usada por el visualizador.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FilterData
{
public:
    enum Type { LP = 0, HP, BP, Notch };

    void prepareToPlay(double sampleRate);
    void reset();

    void setParameters(int type, float cutoffHz, float resonance, float drive);

    float processSample(float sample);

    // Respuesta del filtro a una frecuencia dada (en dB). Usado por la GUI.
    static double getMagnitudeAtFrequency(int type,
                                          double cutoffHz,
                                          double resonance,
                                          double frequencyHz,
                                          double sampleRate);

private:
    void updateCoefficients();

    juce::dsp::IIR::Filter<float> filter;

    double sampleRate = 44100.0;
    int    currentType = LP;
    float  currentCutoff = 1000.0f;
    float  currentResonance = 0.7f;
    float  currentDrive = 1.0f;
};
