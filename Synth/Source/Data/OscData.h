/*
  ==============================================================================

    OscData.h
    Created: 9 May 2026 12:23:04pm
    Author:  Aitor

    Oscilador basado en fase con anti-aliasing por PolyBLEP.
    Permite: ajuste de octava, semi, fine (cents), pulse width,
             modulacion de pitch externa (LFO/ModEnv), hard sync,
             FM (modulacion de fase desde otra senial), y deteccion
             de wrap-around (necesaria para hard sync).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscData
{
public:
    enum WaveType { Sine = 0, Triangle, Saw, Square, Pulse, Noise };

    void prepareToPlay(double newSampleRate);
    void resetPhase();

    void setWaveType(int choice);
    void setBaseMidiNote(int midiNoteNumber);

    void setOctave(int oct);
    void setSemitone(int semi);
    void setFineTuneCents(float cents);
    void setPulseWidth(float pw);
    void setExtraPitchOffset(float semis);  // modulacion externa de pitch (LFO/ModEnv)
    void setDetuneCents(float cents);       // para unison/spread

    // Renderizado muestra a muestra (necesario para FM/Sync)
    // Devuelve true en la muestra en que la fase ha completado un ciclo (wrap-around).
    float renderSample(bool& wrappedThisSample);
    float renderSampleWithPhaseMod(float phaseModRadians, bool& wrappedThisSample);

    // Para hard-sync: avanzar al estado correspondiente sin renderizar
    void hardSyncResetFromMaster();

private:
    static float polyBlep(float t, float dt);
    void recomputePhaseIncrement();

    double sampleRate = 44100.0;
    double phase = 0.0;
    double phaseIncrement = 0.0;

    int   baseMidiNote = 69;
    int   octave       = 0;
    int   semitone     = 0;
    float fineCents    = 0.0f;
    float extraPitchSemis = 0.0f;
    float detuneCents = 0.0f;
    int   waveType     = Sine;
    float pulseWidth   = 0.5f;

    juce::Random random;
};
