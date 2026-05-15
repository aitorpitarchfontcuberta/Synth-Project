/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

SynthAudioProcessorEditor::SynthAudioProcessorEditor(SynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(800, 600);
    addAndMakeVisible(presetBar);

    const auto bgOsc  = juce::Colour(0xff15151f);
    const auto bgMod  = juce::Colour(0xff181520);
    const auto bgEnv  = juce::Colour(0xff141a1c);
    const auto bgFilt = juce::Colour(0xff141d1a);
    const auto bgFx   = juce::Colour(0xff181820);
    const auto bgMast = juce::Colour(0xff1a1818);

    tabs.addTab("Oscillators", bgOsc,  &page1, false);
    tabs.addTab("Modulation",  bgMod,  &page2, false);
    tabs.addTab("Envelope",    bgEnv,  &page3, false);
    tabs.addTab("Filter",      bgFilt, &page5, false);
    tabs.addTab("FX",          bgFx,   &page6, false);
    tabs.addTab("Master",      bgMast, &page4, false);

    tabs.setTabBarDepth(30);
    tabs.setOutline(0);
    tabs.setIndent(0);
    tabs.setColour(juce::TabbedComponent::backgroundColourId, juce::Colour(0xff0a0a12));
    tabs.setColour(juce::TabbedComponent::outlineColourId,    juce::Colours::transparentBlack);

    for (int i = 0; i < tabs.getNumTabs(); ++i)
    {
        if (auto* btn = tabs.getTabbedButtonBar().getTabButton(i))
        {
            btn->setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
            btn->setColour(juce::TextButton::textColourOnId,  juce::Colours::white);
        }
    }

    addAndMakeVisible(tabs);

    // Panel inferior con dos vistas
    const auto bgOut = juce::Colour(0xff0e0e16);
    outputTabs.addTab("Waveform", bgOut, &scope,    false);
    outputTabs.addTab("Spectrum", bgOut, &spectrum, false);
    outputTabs.setTabBarDepth(24);
    outputTabs.setOutline(0);
    outputTabs.setIndent(0);
    outputTabs.setColour(juce::TabbedComponent::backgroundColourId, juce::Colour(0xff0a0a12));
    outputTabs.setColour(juce::TabbedComponent::outlineColourId,    juce::Colours::transparentBlack);

    for (int i = 0; i < outputTabs.getNumTabs(); ++i)
    {
        if (auto* btn = outputTabs.getTabbedButtonBar().getTabButton(i))
        {
            btn->setColour(juce::TextButton::textColourOffId, juce::Colours::lightgrey);
            btn->setColour(juce::TextButton::textColourOnId,  juce::Colours::white);
        }
    }

    addAndMakeVisible(outputTabs);
}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor() {}

void SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient bgGradient(juce::Colour(0xff15151f), bounds.getTopLeft(),
                                    juce::Colour(0xff0a0a12), bounds.getBottomRight(), false);
    g.setGradientFill(bgGradient);
    g.fillRect(bounds);
}

void SynthAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Barra de presets arriba
    const int presetBarH = 34;
    auto presetArea = bounds.removeFromTop(presetBarH);
    presetArea.reduce(6, 4);
    presetBar.setBounds(presetArea);

    // Panel de output abajo
    const int outputHeight = 140;
    auto outputArea = bounds.removeFromBottom(outputHeight);
    outputArea.reduce(6, 6);
    outputTabs.setBounds(outputArea);

    // El resto para las pestanas principales
    tabs.setBounds(bounds);
}
