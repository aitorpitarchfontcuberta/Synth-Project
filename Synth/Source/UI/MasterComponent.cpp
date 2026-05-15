/*
  ==============================================================================

    MasterComponent.cpp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MasterComponent.h"

MasterComponent::MasterComponent(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setText("MASTER / VOICE", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff7a9dff));
    addAndMakeVisible(titleLabel);

    juce::StringArray modes { "Poly","Mono","Legato" };
    voiceModeBox.addItemList(modes, 1);
    voiceModeBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(voiceModeBox);
    setLabelParams(voiceModeLabel, "Voice");

    setSliderParams(gainSlider);
    setSliderParams(velSlider);
    setSliderParams(glideSlider);
    setSliderParams(unisonSlider);
    setSliderParams(detuneSlider);
    setSliderParams(spreadSlider);

    glideSlider.setTextValueSuffix(" s");
    detuneSlider.setTextValueSuffix(" ct");

    setLabelParams(gainLabel,   "Gain");
    setLabelParams(velLabel,    "Vel");
    setLabelParams(glideLabel,  "Glide");
    setLabelParams(unisonLabel, "Unison");
    setLabelParams(detuneLabel, "Detune");
    setLabelParams(spreadLabel, "Spread");

    gainA      = std::make_unique<SA>(apvts, "MASTER_GAIN",    gainSlider);
    velA       = std::make_unique<SA>(apvts, "VEL_SENS",       velSlider);
    glideA     = std::make_unique<SA>(apvts, "GLIDE",          glideSlider);
    unisonA    = std::make_unique<SA>(apvts, "UNISON_VOICES",  unisonSlider);
    detuneA    = std::make_unique<SA>(apvts, "UNISON_DETUNE",  detuneSlider);
    spreadA    = std::make_unique<SA>(apvts, "UNISON_SPREAD",  spreadSlider);
    voiceModeA = std::make_unique<CA>(apvts, "VOICE_MODE",     voiceModeBox);

    velVis    = std::make_unique<VelocityCurveVisualizer>(apvts);
    glideVis  = std::make_unique<GlideVisualizer>(apvts);
    unisonVis = std::make_unique<UnisonVisualizer>(apvts);
    addAndMakeVisible(*velVis);
    addAndMakeVisible(*glideVis);
    addAndMakeVisible(*unisonVis);
}

MasterComponent::~MasterComponent() {}

void MasterComponent::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat().reduced(2.0f);
    juce::ColourGradient grad(juce::Colour(0xff2a2a3a), b.getTopLeft(),
                              juce::Colour(0xff1a1a25), b.getBottomRight(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(b, 8.0f);
    g.setColour(juce::Colour(0xff4a4a6a));
    g.drawRoundedRectangle(b, 8.0f, 1.5f);
}

void MasterComponent::resized()
{
    auto b = getLocalBounds().reduced(8);

    // Cabecera: title izq, voice mode der
    auto headRow = b.removeFromTop(24);
    titleLabel.setBounds(headRow.removeFromLeft(headRow.getWidth() / 2));
    voiceModeLabel.setBounds(headRow.removeFromLeft(40));
    voiceModeBox.setBounds(headRow);

    b.removeFromTop(4);

    // Fila de 3 mini-visualizadores: Velocity | Glide | Unison
    auto visRow = b.removeFromTop(80);
    {
        const int gap = 6;
        const int colW = (visRow.getWidth() - gap * 2) / 3;
        if (velVis)    velVis   ->setBounds(visRow.removeFromLeft(colW));
        visRow.removeFromLeft(gap);
        if (glideVis)  glideVis ->setBounds(visRow.removeFromLeft(colW));
        visRow.removeFromLeft(gap);
        if (unisonVis) unisonVis->setBounds(visRow);
    }

    b.removeFromTop(6);

    // 6 knobs en una fila
    const int labelH = 14;
    const int n = 6;
    const int w = b.getWidth() / n;
    auto layout = [&](juce::Slider& s, juce::Label& l, int i)
    {
        auto a = juce::Rectangle<int>(b.getX() + i * w, b.getY(), w, b.getHeight());
        l.setBounds(a.removeFromBottom(labelH));
        s.setBounds(a.reduced(2));
    };
    layout(gainSlider,   gainLabel,   0);
    layout(velSlider,    velLabel,    1);
    layout(glideSlider,  glideLabel,  2);
    layout(unisonSlider, unisonLabel, 3);
    layout(detuneSlider, detuneLabel, 4);
    layout(spreadSlider, spreadLabel, 5);
}

void MasterComponent::setSliderParams(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
    s.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xfffff066));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a50));
    s.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
    s.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    s.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    addAndMakeVisible(s);
}

void MasterComponent::setLabelParams(juce::Label& l, const juce::String& t)
{
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(10.5f, juce::Font::bold));
    l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(l);
}
