/*
  ==============================================================================

    AdsrVisualizer.h
    Author:  Aitor

    Dibuja la curva A-D-S-R en tiempo real leyendo los valores del APVTS.
    Configurable mediante los IDs de los 4 parametros (para amp env y mod env).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AdsrVisualizer : public juce::Component, private juce::Timer
{
public:
    AdsrVisualizer(juce::AudioProcessorValueTreeState& apvts,
                   juce::String attackID  = "ATTACK",
                   juce::String decayID   = "DECAY",
                   juce::String sustainID = "SUSTAIN",
                   juce::String releaseID = "RELEASE",
                   juce::Colour curveColour = juce::Colour(0xff7a9dff));
    ~AdsrVisualizer() override;

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& apvtsRef;
    juce::String aID, dID, sID, rID;
    juce::Colour colour;

    float lastA = -1.0f, lastD = -1.0f, lastS = -1.0f, lastR = -1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdsrVisualizer)
};
