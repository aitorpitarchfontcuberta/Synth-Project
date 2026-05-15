/*
  ==============================================================================

    LfoData.cpp

  ==============================================================================
*/

#include "LfoData.h"

void LfoData::prepareToPlay(double newSampleRate)
{
    sampleRate = newSampleRate;
    phase = 0.0;
    phaseIncrement = (double)rate / sampleRate;
}

void LfoData::resetPhase()
{
    phase = 0.0;
    shValue = random.nextFloat() * 2.0f - 1.0f;
}

void LfoData::setWaveType(int choice)
{
    waveType = juce::jlimit(0, (int)SampleHold, choice);
}

void LfoData::setRateHz(float rateHz)
{
    rate = juce::jlimit(0.01f, 30.0f, rateHz);
    phaseIncrement = (double)rate / sampleRate;
}

float LfoData::renderSample()
{
    const float t = (float)phase;
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
            break;
        case Square:
            sample = (t < 0.5f) ? 1.0f : -1.0f;
            break;
        case SampleHold:
            sample = shValue;
            break;
    }

    // Avanzar fase; en S&H, refrescar valor al cruzar 0
    phase += phaseIncrement;
    if (phase >= 1.0)
    {
        phase -= 1.0;
        if (waveType == SampleHold)
            shValue = random.nextFloat() * 2.0f - 1.0f;
    }

    return sample;
}
