/*
  ==============================================================================

    LfoComponent.cpp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LfoComponent.h"

LfoComponent::LfoComponent(juce::AudioProcessorValueTreeState& apvts,
                           const juce::String& titleText,
                           const juce::String& paramPrefix)
    : title(titleText)
{
    titleLabel.setText(title, juce::dontSendNotification);
    titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffffaa55));
    addAndMakeVisible(titleLabel);

    juce::StringArray waves { "Sine","Triangle","Saw","Square","S&H" };
    juce::StringArray dests { "None","Pitch","Osc1 PW","Osc2 PW","Amp","Mix","FM Amt","Cutoff","Reso" };

    waveBox.addItemList(waves, 1);
    destBox.addItemList(dests, 1);
    waveBox.setJustificationType(juce::Justification::centred);
    destBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(waveBox);
    addAndMakeVisible(destBox);

    setLabelParams(waveLabel, "Wave");
    setLabelParams(destLabel, "Dest");

    setSliderParams(rateSlider);
    setSliderParams(depthSlider);
    rateSlider.setTextValueSuffix(" Hz");
    setLabelParams(rateLabel,  "Rate");
    setLabelParams(depthLabel, "Depth");

    waveAttach  = std::make_unique<CA>(apvts, paramPrefix + "_WAVE",  waveBox);
    destAttach  = std::make_unique<CA>(apvts, paramPrefix + "_DEST",  destBox);
    rateAttach  = std::make_unique<SA>(apvts, paramPrefix + "_RATE",  rateSlider);
    depthAttach = std::make_unique<SA>(apvts, paramPrefix + "_DEPTH", depthSlider);

    waveVis = std::make_unique<LfoVisualizer>(
        apvts,
        paramPrefix + "_WAVE",
        paramPrefix + "_RATE",
        paramPrefix + "_DEPTH",
        juce::Colour(0xffffaa55));
    addAndMakeVisible(*waveVis);
}

LfoComponent::~LfoComponent() {}

void LfoComponent::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat().reduced(2.0f);
    juce::ColourGradient grad(juce::Colour(0xff2a2a3a), b.getTopLeft(),
                              juce::Colour(0xff1a1a25), b.getBottomRight(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(b, 8.0f);
    g.setColour(juce::Colour(0xff4a4a6a));
    g.drawRoundedRectangle(b, 8.0f, 1.5f);
}

void LfoComponent::resized()
{
    auto b = getLocalBounds().reduced(8);
    titleLabel.setBounds(b.removeFromTop(18));
    b.removeFromTop(4);

    // Fila wave
    {
        auto row = b.removeFromTop(20);
        waveLabel.setBounds(row.removeFromLeft(40));
        waveBox.setBounds(row);
    }
    b.removeFromTop(3);

    // Fila dest
    {
        auto row = b.removeFromTop(20);
        destLabel.setBounds(row.removeFromLeft(40));
        destBox.setBounds(row);
    }
    b.removeFromTop(6);

    // Visualizador
    auto visArea = b.removeFromTop(60);
    if (waveVis) waveVis->setBounds(visArea);

    b.removeFromTop(6);

    // Knobs Rate y Depth - 2 columnas
    const int labelH = 16;
    const int w = b.getWidth() / 2;

    auto rateArea  = juce::Rectangle<int>(b.getX(),     b.getY(), w, b.getHeight());
    auto depthArea = juce::Rectangle<int>(b.getX() + w, b.getY(), w, b.getHeight());

    rateLabel.setBounds(rateArea.removeFromBottom(labelH));
    rateSlider.setBounds(rateArea.reduced(2));
    depthLabel.setBounds(depthArea.removeFromBottom(labelH));
    depthSlider.setBounds(depthArea.reduced(2));
}

void LfoComponent::setSliderParams(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
    s.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xffffaa55));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a50));
    s.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
    s.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    s.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    addAndMakeVisible(s);
}

void LfoComponent::setLabelParams(juce::Label& l, const juce::String& t)
{
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(11.5f, juce::Font::bold));
    l.setJustificationType(juce::Justification::centredLeft);
    l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(l);
}
