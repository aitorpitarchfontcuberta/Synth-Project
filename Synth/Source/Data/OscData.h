/*
  ==============================================================================

    OscData.h
    Created: 9 May 2026 12:23:04pm
    Author:  Aitor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscData : public juce::dsp::Oscillator<float>
{
public:
	void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void setWaveType(const int choice);
	void setWaveFrequency(const int midiNoteNumber);
    void getNextAudioBlock(juce::dsp::AudioBlock<float>& block);


private:


};