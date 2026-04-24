/*
  ==============================================================================

    SynthVoice.h
    Created: 23 Apr 2026 2:12:30pm
    Author:  aitor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound* sound,
		            int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
	void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

private:
	juce::ADSR adsr;
	juce::ADSR::Parameters adsrParams;


    juce::dsp::Oscillator<float> osc{ [](float x) { return x < 0.0f ? -1.0f : 1.0f; }, 200 };
    juce::dsp::Gain<float> gain;
	bool isPrepared{ false };

    // return std::sin(x);                               //sinus
    // return x / juce::MathConstants<float>::pi;        //serra
    // return x < 0.0f ? -1.0f : 1.0f;                   //square
};