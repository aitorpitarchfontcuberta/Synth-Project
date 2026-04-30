/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthAudioProcessorEditor::SynthAudioProcessorEditor (SynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), adsr (audioProcessor.apvts)
{
    setSize (400, 300);

    
    oscSelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "OSC", oscSelector);
	
    addAndMakeVisible(adsr);
}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor()
{
}

//==============================================================================
void SynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void SynthAudioProcessorEditor::resized()
{
	adsr.setBounds(getWidth()/2, 0, getWidth()/2, getHeight());
}

