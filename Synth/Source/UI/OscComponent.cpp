/*
  ==============================================================================

    OscComponent.cpp
    Created: 12 May 2026 9:34:43pm
    Author:  Aitor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscComponent.h"

//==============================================================================
OscComponent::OscComponent(juce::AudioProcessorValueTreeState& apvts, juce::String waveSelectorID)
{
	juce::StringArray choices{ "Sine", "Saw", "Square" };
	oscWaveSelector.addItemList(choices, 1);
	addAndMakeVisible(oscWaveSelector);

	oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, waveSelectorID, oscWaveSelector);
}

OscComponent::~OscComponent()
{
}

void OscComponent::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);
}

void OscComponent::resized()
{
	oscWaveSelector.setBounds(0, 0, 90, 20);

}

