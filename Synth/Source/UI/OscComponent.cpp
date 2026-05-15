/*
  ==============================================================================

    OscComponent.cpp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscComponent.h"

OscComponent::OscComponent(juce::AudioProcessorValueTreeState& apvts,
                           const juce::String& titleText,
                           const juce::String& paramPrefix)
    : title(titleText)
{
    titleLabel.setText(title, juce::dontSendNotification);
    titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff7a9dff));
    addAndMakeVisible(titleLabel);

    waveLabel.setText("Wave", juce::dontSendNotification);
    waveLabel.setFont(juce::Font(11.5f, juce::Font::bold));
    waveLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(waveLabel);

    juce::StringArray choices{ "Sine", "Triangle", "Saw", "Square", "Pulse", "Noise" };
    oscWaveSelector.addItemList(choices, 1);
    oscWaveSelector.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(oscWaveSelector);
    oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, paramPrefix + "WAVETYPE", oscWaveSelector);

    waveVis = std::make_unique<WaveformVisualizer>(
        apvts,
        paramPrefix + "WAVETYPE",
        paramPrefix + "PW",
        juce::Colour(0xff7a9dff),
        WaveformVisualizer::Mode::Oscillator,
        paramPrefix + "OCTAVE",
        paramPrefix + "SEMI",
        paramPrefix + "FINE");
    addAndMakeVisible(*waveVis);

    setSliderParams(octaveSlider);
    setSliderParams(semiSlider);
    setSliderParams(fineSlider);
    setSliderParams(pwSlider);

    semiSlider.setTextValueSuffix(" st");
    fineSlider.setTextValueSuffix(" ct");

    setLabelParams(octaveLabel, "Octave");
    setLabelParams(semiLabel,   "Semi");
    setLabelParams(fineLabel,   "Fine");
    setLabelParams(pwLabel,     "Pulse W");

    octaveAttachment = std::make_unique<SliderAttachment>(apvts, paramPrefix + "OCTAVE", octaveSlider);
    semiAttachment   = std::make_unique<SliderAttachment>(apvts, paramPrefix + "SEMI",   semiSlider);
    fineAttachment   = std::make_unique<SliderAttachment>(apvts, paramPrefix + "FINE",   fineSlider);
    pwAttachment     = std::make_unique<SliderAttachment>(apvts, paramPrefix + "PW",     pwSlider);
}

OscComponent::~OscComponent() {}

void OscComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    juce::ColourGradient grad(juce::Colour(0xff2a2a3a), bounds.getTopLeft(),
                              juce::Colour(0xff1a1a25), bounds.getBottomRight(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(juce::Colour(0xff4a4a6a));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void OscComponent::resized()
{
    auto bounds = getLocalBounds().reduced(8);

    titleLabel.setBounds(bounds.removeFromTop(18));
    bounds.removeFromTop(4);

    // Fila wave (label + combo en la misma fila)
    {
        auto top = bounds.removeFromTop(24);
        waveLabel.setBounds(top.removeFromLeft(48));
        oscWaveSelector.setBounds(top);
    }

    bounds.removeFromTop(6);

    // Visualizador de la forma de onda (mas grande)
    auto visArea = bounds.removeFromTop(80);
    if (waveVis) waveVis->setBounds(visArea);

    bounds.removeFromTop(8);

    // Knobs en disposicion 2x2 para que sean mas grandes
    const int labelH = 16;
    const int colW = bounds.getWidth() / 2;
    const int rowH = bounds.getHeight() / 2;

    auto layout = [&](juce::Slider& s, juce::Label& l, int col, int row)
    {
        auto a = juce::Rectangle<int>(bounds.getX() + col * colW,
                                      bounds.getY() + row * rowH,
                                      colW, rowH);
        l.setBounds(a.removeFromBottom(labelH));
        s.setBounds(a.reduced(2));
    };

    layout(octaveSlider, octaveLabel, 0, 0);
    layout(semiSlider,   semiLabel,   1, 0);
    layout(fineSlider,   fineLabel,   0, 1);
    layout(pwSlider,     pwLabel,     1, 1);
}

void OscComponent::setSliderParams(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
    s.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xff7a9dff));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a50));
    s.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
    s.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    s.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    addAndMakeVisible(s);
}

void OscComponent::setLabelParams(juce::Label& l, const juce::String& text)
{
    l.setText(text, juce::dontSendNotification);
    l.setFont(juce::Font(11.5f, juce::Font::bold));
    l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(l);
}
