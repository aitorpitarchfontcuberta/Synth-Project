/*
  ==============================================================================

    SynthSound.h
    Created: 23 Apr 2026 2:13:08pm
    Author:  aitor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int midiNoteNumber) override { return true; }
    bool appliesToChannel (int midiChannel) override { return true; }
};