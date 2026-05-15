/*
  ==============================================================================

    AdsrComponent.cpp
    Created: 30 Apr 2026 11:36:06am
    Author:  Aitor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AdsrComponent.h"

//==============================================================================
AdsrComponent::AdsrComponent(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setText("AMP ENV", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff7a9dff));
    addAndMakeVisible(titleLabel);

    // Visualizador en tiempo real
    visualizer = std::make_unique<AdsrVisualizer>(apvts);
    addAndMakeVisible(*visualizer);

    // Etiquetas de cada knob
    setLabelParams(attackLabel,  "Attack");
    setLabelParams(decayLabel,   "Decay");
    setLabelParams(sustainLabel, "Sustain");
    setLabelParams(releaseLabel, "Release");

    // Sliders
    setSliderParams(attackSlider);
    setSliderParams(decaySlider);
    setSliderParams(sustainSlider);
    setSliderParams(releaseSlider);

    attackSlider.setTextValueSuffix(" s");
    decaySlider.setTextValueSuffix(" s");
    releaseSlider.setTextValueSuffix(" s");

    attackAttachment  = std::make_unique<SliderAttachment>(apvts, "ATTACK",  attackSlider);
    decayAttachment   = std::make_unique<SliderAttachment>(apvts, "DECAY",   decaySlider);
    sustainAttachment = std::make_unique<SliderAttachment>(apvts, "SUSTAIN", sustainSlider);
    releaseAttachment = std::make_unique<SliderAttachment>(apvts, "RELEASE", releaseSlider);
}

AdsrComponent::~AdsrComponent()
{
}

void AdsrComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    juce::ColourGradient gradient(juce::Colour(0xff2a2a3a), bounds.getTopLeft(),
                                  juce::Colour(0xff1a1a25), bounds.getBottomRight(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);

    g.setColour(juce::Colour(0xff4a4a6a));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void AdsrComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    titleLabel.setBounds(bounds.removeFromTop(18));
    bounds.removeFromTop(4);

    // El visualizador ocupa la parte superior (40% de la altura restante)
    auto visualArea = bounds.removeFromTop((int)(bounds.getHeight() * 0.4f));
    visualizer->setBounds(visualArea);

    bounds.removeFromTop(6);

    // Los knobs ocupan la parte inferior
    const int labelHeight = 18;
    const int numKnobs = 4;
    const int knobAreaWidth = bounds.getWidth() / numKnobs;

    auto layoutKnob = [&](juce::Slider& slider, juce::Label& label, int index)
    {
        auto area = juce::Rectangle<int>(bounds.getX() + index * knobAreaWidth,
                                         bounds.getY(),
                                         knobAreaWidth,
                                         bounds.getHeight());
        auto labelArea = area.removeFromBottom(labelHeight);
        label.setBounds(labelArea);
        slider.setBounds(area.reduced(4));
    };

    layoutKnob(attackSlider,  attackLabel,  0);
    layoutKnob(decaySlider,   decayLabel,   1);
    layoutKnob(sustainSlider, sustainLabel, 2);
    layoutKnob(releaseSlider, releaseLabel, 3);
}

void AdsrComponent::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    slider.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xff7a9dff));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a50));
    slider.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    addAndMakeVisible(slider);
}

void AdsrComponent::setLabelParams(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(12.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(label);
}
