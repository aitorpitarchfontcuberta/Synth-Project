/*
  ==============================================================================

    FilterData.cpp

  ==============================================================================
*/

#include "FilterData.h"

void FilterData::prepareToPlay(double newSampleRate)
{
    sampleRate = newSampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = 512;
    spec.numChannels = 1;
    filter.prepare(spec);

    updateCoefficients();
}

void FilterData::reset()
{
    filter.reset();
}

void FilterData::setParameters(int type, float cutoffHz, float resonance, float drive)
{
    const bool changed =
        type != currentType
        || cutoffHz   != currentCutoff
        || resonance  != currentResonance;

    currentType       = juce::jlimit(0, (int)Notch, type);
    currentCutoff     = juce::jlimit(20.0f, (float)(sampleRate * 0.45), cutoffHz);
    currentResonance  = juce::jlimit(0.1f, 12.0f, resonance);
    currentDrive      = juce::jlimit(1.0f, 12.0f, drive);

    if (changed)
        updateCoefficients();
}

void FilterData::updateCoefficients()
{
    juce::dsp::IIR::Coefficients<float>::Ptr coefs;
    switch (currentType)
    {
        case LP:    coefs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, currentCutoff, currentResonance);  break;
        case HP:    coefs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, currentCutoff, currentResonance); break;
        case BP:    coefs = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, currentCutoff, currentResonance); break;
        case Notch: coefs = juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, currentCutoff, currentResonance);    break;
    }
    *filter.coefficients = *coefs;
}

float FilterData::processSample(float sample)
{
    // Drive: saturacion tanh suave para que la senial entre con caracter analogico
    if (currentDrive > 1.0001f)
    {
        const float drivenInput = sample * currentDrive;
        sample = std::tanh(drivenInput) / std::tanh(currentDrive);
    }
    return filter.processSample(sample);
}

double FilterData::getMagnitudeAtFrequency(int type,
                                           double cutoffHz,
                                           double resonance,
                                           double frequencyHz,
                                           double sampleRate)
{
    juce::dsp::IIR::Coefficients<double>::Ptr c;
    switch (type)
    {
        case LP:    c = juce::dsp::IIR::Coefficients<double>::makeLowPass(sampleRate, cutoffHz, resonance);  break;
        case HP:    c = juce::dsp::IIR::Coefficients<double>::makeHighPass(sampleRate, cutoffHz, resonance); break;
        case BP:    c = juce::dsp::IIR::Coefficients<double>::makeBandPass(sampleRate, cutoffHz, resonance); break;
        case Notch: c = juce::dsp::IIR::Coefficients<double>::makeNotch(sampleRate, cutoffHz, resonance);    break;
        default:    return 1.0;
    }
    return c->getMagnitudeForFrequency(frequencyHz, sampleRate);
}
