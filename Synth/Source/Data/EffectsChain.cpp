/*
  ==============================================================================

    EffectsChain.cpp

  ==============================================================================
*/

#include "EffectsChain.h"

void EffectsChain::prepareToPlay(double sr, int bs, int numChannels)
{
    sampleRate = sr;
    blockSize  = bs;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sr;
    spec.maximumBlockSize = (juce::uint32) bs;
    spec.numChannels = (juce::uint32) numChannels;

    chorus.prepare(spec);
    chorus.reset();
    chorus.setMix(0.0f);

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    // 2 segundos maximo de delay
    const int maxDelaySamples = (int) (sr * 2.0);
    delayL.setMaximumDelayInSamples(maxDelaySamples);
    delayR.setMaximumDelayInSamples(maxDelaySamples);
    delayL.prepare(monoSpec);
    delayR.prepare(monoSpec);
    delayL.reset();
    delayR.reset();

    delayTimeSmoothed.reset(sr, 0.05);
    delayMixSmoothed .reset(sr, 0.05);
    delayFbSmoothed  .reset(sr, 0.05);
    delayTimeSmoothed.setCurrentAndTargetValue(delayTimeSamples);
    delayMixSmoothed .setCurrentAndTargetValue(delayMix);
    delayFbSmoothed  .setCurrentAndTargetValue(delayFeedback);

    reverb.prepare(spec);
    reverb.reset();
}

void EffectsChain::reset()
{
    chorus.reset();
    delayL.reset();
    delayR.reset();
    reverb.reset();
}

void EffectsChain::setChorusParams(float rate, float depth, float feedback, float mix)
{
    chorus.setRate(rate);
    chorus.setDepth(depth);
    chorus.setFeedback(feedback);
    chorus.setMix(mix);
    // Centre delay fijo en 7 ms (estandar para chorus suave).
    chorus.setCentreDelay(7.0f);
}

void EffectsChain::setDelayParams(float timeMs, float feedback, float mix)
{
    delayTimeSamples = (float) ((double) timeMs * 0.001 * sampleRate);
    delayFeedback    = juce::jlimit(0.0f, 0.95f, feedback);
    delayMix         = juce::jlimit(0.0f, 1.0f, mix);

    delayTimeSmoothed.setTargetValue(delayTimeSamples);
    delayFbSmoothed  .setTargetValue(delayFeedback);
    delayMixSmoothed .setTargetValue(delayMix);
}

void EffectsChain::setReverbParams(float size, float damping, float width, float mix)
{
    reverbParams.roomSize  = juce::jlimit(0.0f, 1.0f, size);
    reverbParams.damping   = juce::jlimit(0.0f, 1.0f, damping);
    reverbParams.width     = juce::jlimit(0.0f, 1.0f, width);
    reverbParams.wetLevel  = juce::jlimit(0.0f, 1.0f, mix);
    reverbParams.dryLevel  = juce::jlimit(0.0f, 1.0f, 1.0f - mix * 0.5f);
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);
    reverbMix = mix;
}

void EffectsChain::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();
    if (numChannels < 1 || numSamples < 1) return;

    // === Chorus ===
    if (chorusOn)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        chorus.process(ctx);
    }

    // === Delay (estereo independiente por canal) ===
    if (delayOn)
    {
        auto* L = buffer.getWritePointer(0);
        auto* R = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            const float t   = delayTimeSmoothed.getNextValue();
            const float fb  = delayFbSmoothed  .getNextValue();
            const float mix = delayMixSmoothed .getNextValue();

            const float drySL = L[i];
            const float drySR = R != nullptr ? R[i] : drySL;

            delayL.setDelay(t);
            delayR.setDelay(t);

            const float wetL = delayL.popSample(0);
            const float wetR = delayR.popSample(0);

            delayL.pushSample(0, drySL + wetL * fb);
            delayR.pushSample(0, drySR + wetR * fb);

            L[i] = drySL * (1.0f - mix) + wetL * mix;
            if (R != nullptr) R[i] = drySR * (1.0f - mix) + wetR * mix;
        }
    }

    // === Reverb ===
    if (reverbOn)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        reverb.process(ctx);
    }
}
