/*
  ==============================================================================

    OscData.cpp
    Created: 9 May 2026 12:23:04pm
    Author:  Aitor

  ==============================================================================
*/

#include "OscData.h"

void OscData::prepareToPlay(double newSampleRate)
{
    sampleRate = newSampleRate;
    phase = 0.0;
    recomputePhaseIncrement();
}

void OscData::resetPhase()
{
    phase = 0.0;
}

void OscData::hardSyncResetFromMaster()
{
    phase = 0.0;
}

void OscData::setWaveType(int choice)        { waveType = juce::jlimit(0, (int)Noise, choice); }
void OscData::setBaseMidiNote(int n)         { baseMidiNote = n; recomputePhaseIncrement(); }
void OscData::setOctave(int oct)             { octave = juce::jlimit(-4, 4, oct); recomputePhaseIncrement(); }
void OscData::setSemitone(int semi)          { semitone = juce::jlimit(-12, 12, semi); recomputePhaseIncrement(); }
void OscData::setFineTuneCents(float cents)  { fineCents = juce::jlimit(-100.0f, 100.0f, cents); recomputePhaseIncrement(); }
void OscData::setExtraPitchOffset(float s)   { extraPitchSemis = s; recomputePhaseIncrement(); }
void OscData::setDetuneCents(float cents)    { detuneCents = cents; recomputePhaseIncrement(); }
void OscData::setPulseWidth(float pw)        { pulseWidth = juce::jlimit(0.05f, 0.95f, pw); }

void OscData::recomputePhaseIncrement()
{
    const double baseHz = juce::MidiMessage::getMidiNoteInHertz(baseMidiNote);
    const double semis  = (double)(octave * 12 + semitone)
                        + (double)fineCents / 100.0
                        + (double)detuneCents / 100.0
                        + (double)extraPitchSemis;
    const double freq = baseHz * std::pow(2.0, semis / 12.0);
    phaseIncrement = freq / sampleRate;
}

float OscData::polyBlep(float t, float dt)
{
    if (t < dt)         { float x = t / dt;          return x + x - x * x - 1.0f; }
    if (t > 1.0f - dt)  { float x = (t - 1.0f) / dt; return x * x + x + x + 1.0f; }
    return 0.0f;
}

float OscData::renderSample(bool& wrappedThisSample)
{
    return renderSampleWithPhaseMod(0.0f, wrappedThisSample);
}

float OscData::renderSampleWithPhaseMod(float phaseModRadians, bool& wrappedThisSample)
{
    // Convertir la modulacion de fase (en radianes) a unidades [0,1)
    float pmNorm = phaseModRadians / juce::MathConstants<float>::twoPi;

    float t  = (float)phase + pmNorm;
    // Mantener t en [0,1) para el calculo de la forma de onda
    t -= std::floor(t);
    const float dt = (float)phaseIncrement;

    float sample = 0.0f;

    switch (waveType)
    {
        case Sine:
            sample = std::sin(t * juce::MathConstants<float>::twoPi);
            break;
        case Triangle:
            sample = (t < 0.5f) ? (-1.0f + 4.0f * t)
                                : ( 3.0f - 4.0f * t);
            break;
        case Saw:
            sample = 2.0f * t - 1.0f;
            sample -= polyBlep(t, dt);
            break;
        case Square:
        {
            sample = (t < 0.5f) ? 1.0f : -1.0f;
            sample += polyBlep(t, dt);
            float t2 = t + 0.5f; if (t2 >= 1.0f) t2 -= 1.0f;
            sample -= polyBlep(t2, dt);
            break;
        }
        case Pulse:
        {
            sample = (t < pulseWidth) ? 1.0f : -1.0f;
            sample += polyBlep(t, dt);
            float t2 = t + (1.0f - pulseWidth); if (t2 >= 1.0f) t2 -= 1.0f;
            sample -= polyBlep(t2, dt);
            break;
        }
        case Noise:
            sample = random.nextFloat() * 2.0f - 1.0f;
            break;
    }

    // Avanzar la fase y detectar el wrap-around (necesario para hard-sync)
    const double prev = phase;
    phase += phaseIncrement;
    wrappedThisSample = false;
    while (phase >= 1.0)
    {
        phase -= 1.0;
        wrappedThisSample = true;
    }
    juce::ignoreUnused(prev);

    return sample;
}
