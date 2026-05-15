/*
  ==============================================================================

    MixerComponent.cpp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MixerComponent.h"

MixerComponent::MixerComponent(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setText("MIXER", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff7a9dff));
    addAndMakeVisible(titleLabel);

    juce::StringArray modes { "Mix","Ring","Sync","FM" };
    modeBox.addItemList(modes, 1);
    modeBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modeBox);
    setLabelParams(modeLabel, "Mode");

    juce::StringArray subWaves { "Sine","Triangle","Saw","Square","Pulse","Noise" };
    subWaveBox.addItemList(subWaves, 1);
    subWaveBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(subWaveBox);
    setLabelParams(subWaveLabel, "Sub Wave");

    setSliderParams(mixSlider,      juce::Colour(0xff7a9dff));
    setSliderParams(fmSlider,       juce::Colour(0xffffaa55));
    setSliderParams(subLevelSlider, juce::Colour(0xffff7a9d));

    setLabelParams(mixLabel,      "Osc Mix");
    setLabelParams(fmLabel,       "FM Amt");
    setLabelParams(subLevelLabel, "Sub Lvl");

    mixA      = std::make_unique<SA>(apvts, "OSC_MIX",   mixSlider);
    fmA       = std::make_unique<SA>(apvts, "FM_AMOUNT", fmSlider);
    subLevelA = std::make_unique<SA>(apvts, "SUB_LEVEL", subLevelSlider);
    modeA     = std::make_unique<CA>(apvts, "OSC_MODE",  modeBox);
    subWaveA  = std::make_unique<CA>(apvts, "SUB_WAVE",  subWaveBox);

    mixerVis = std::make_unique<MixerVisualizer>(apvts);
    addAndMakeVisible(*mixerVis);

    mixBar = std::make_unique<MixBarVisualizer>(apvts, "OSC_MIX");
    addAndMakeVisible(*mixBar);
}

MixerComponent::~MixerComponent() {}

void MixerComponent::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat().reduced(2.0f);
    juce::ColourGradient grad(juce::Colour(0xff2a2a3a), b.getTopLeft(),
                              juce::Colour(0xff1a1a25), b.getBottomRight(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(b, 8.0f);
    g.setColour(juce::Colour(0xff4a4a6a));
    g.drawRoundedRectangle(b, 8.0f, 1.5f);
}

void MixerComponent::resized()
{
    auto b = getLocalBounds().reduced(8);
    titleLabel.setBounds(b.removeFromTop(18));
    b.removeFromTop(4);

    // Fila Mode (label arriba + combo abajo)
    {
        auto modeRow = b.removeFromTop(20);
        modeLabel.setBounds(modeRow.removeFromLeft(50));
        modeBox.setBounds(modeRow);
    }
    b.removeFromTop(4);

    // Fila Sub Wave
    {
        auto subRow = b.removeFromTop(20);
        subWaveLabel.setBounds(subRow.removeFromLeft(50));
        subWaveBox.setBounds(subRow);
    }
    b.removeFromTop(6);

    // Visualizador de la forma resultante (grande)
    auto visArea = b.removeFromTop(70);
    if (mixerVis) mixerVis->setBounds(visArea);

    b.removeFromTop(4);

    // Barra visual del mix (mas fina)
    auto barArea = b.removeFromTop(18);
    if (mixBar) mixBar->setBounds(barArea);

    b.removeFromTop(6);

    // 3 knobs
    const int labelH = 16;
    const int n = 3;
    const int w = b.getWidth() / n;
    auto layout = [&](juce::Slider& s, juce::Label& l, int i)
    {
        auto a = juce::Rectangle<int>(b.getX() + i * w, b.getY(), w, b.getHeight());
        l.setBounds(a.removeFromBottom(labelH));
        s.setBounds(a.reduced(2));
    };
    layout(mixSlider,      mixLabel,      0);
    layout(fmSlider,       fmLabel,       1);
    layout(subLevelSlider, subLevelLabel, 2);
}

void MixerComponent::setSliderParams(juce::Slider& s, juce::Colour colour)
{
    s.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
    s.setColour(juce::Slider::rotarySliderFillColourId,    colour);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a50));
    s.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
    s.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    s.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    addAndMakeVisible(s);
}

void MixerComponent::setLabelParams(juce::Label& l, const juce::String& t)
{
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(11.5f, juce::Font::bold));
    l.setJustificationType(juce::Justification::centredLeft);
    l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(l);
}
