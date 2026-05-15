/*
  ==============================================================================

    ModEnvComponent.cpp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ModEnvComponent.h"

ModEnvComponent::ModEnvComponent(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setText("MOD ENV", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8effc1));
    addAndMakeVisible(titleLabel);

    setSliderParams(attackSlider);
    setSliderParams(decaySlider);
    setSliderParams(sustainSlider);
    setSliderParams(releaseSlider);
    setSliderParams(depthSlider);
    attackSlider.setTextValueSuffix(" s");
    decaySlider.setTextValueSuffix(" s");
    releaseSlider.setTextValueSuffix(" s");

    setLabelParams(attackLabel,  "Attack");
    setLabelParams(decayLabel,   "Decay");
    setLabelParams(sustainLabel, "Sustain");
    setLabelParams(releaseLabel, "Release");
    setLabelParams(depthLabel,   "Depth");

    juce::StringArray dests { "None","Pitch","Osc1 PW","Osc2 PW","Amp","Mix","FM Amt","Cutoff","Reso" };
    destBox.addItemList(dests, 1);
    destBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(destBox);
    setLabelParams(destLabel, "Dest");

    aA     = std::make_unique<SA>(apvts, "MENV_ATTACK",  attackSlider);
    dA     = std::make_unique<SA>(apvts, "MENV_DECAY",   decaySlider);
    sA     = std::make_unique<SA>(apvts, "MENV_SUSTAIN", sustainSlider);
    rA     = std::make_unique<SA>(apvts, "MENV_RELEASE", releaseSlider);
    depthA = std::make_unique<SA>(apvts, "MENV_DEPTH",   depthSlider);
    destA  = std::make_unique<CA>(apvts, "MENV_DEST",    destBox);

    visualizer = std::make_unique<AdsrVisualizer>(
        apvts, "MENV_ATTACK", "MENV_DECAY", "MENV_SUSTAIN", "MENV_RELEASE",
        juce::Colour(0xff8effc1));
    addAndMakeVisible(*visualizer);
}

ModEnvComponent::~ModEnvComponent() {}

void ModEnvComponent::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat().reduced(2.0f);
    juce::ColourGradient grad(juce::Colour(0xff2a2a3a), b.getTopLeft(),
                              juce::Colour(0xff1a1a25), b.getBottomRight(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(b, 8.0f);
    g.setColour(juce::Colour(0xff4a4a6a));
    g.drawRoundedRectangle(b, 8.0f, 1.5f);
}

void ModEnvComponent::resized()
{
    auto b = getLocalBounds().reduced(8);
    titleLabel.setBounds(b.removeFromTop(18));
    b.removeFromTop(4);

    // Fila Dest (label arriba + combo)
    {
        auto destRow = b.removeFromTop(20);
        destLabel.setBounds(destRow.removeFromLeft(50));
        destBox.setBounds(destRow);
    }

    b.removeFromTop(4);

    // Visualizador
    auto visArea = b.removeFromTop(70);
    if (visualizer) visualizer->setBounds(visArea);

    b.removeFromTop(6);

    const int labelH = 16;

    // Fila 1: A D S R (4 knobs)
    auto row1 = b.removeFromTop((int)(b.getHeight() * 0.5f));
    {
        const int n = 4;
        const int w = row1.getWidth() / n;
        auto layout = [&](juce::Slider& s, juce::Label& l, int i)
        {
            auto a = juce::Rectangle<int>(row1.getX() + i * w, row1.getY(), w, row1.getHeight());
            l.setBounds(a.removeFromBottom(labelH));
            s.setBounds(a.reduced(2));
        };
        layout(attackSlider,  attackLabel,  0);
        layout(decaySlider,   decayLabel,   1);
        layout(sustainSlider, sustainLabel, 2);
        layout(releaseSlider, releaseLabel, 3);
    }

    b.removeFromTop(4);

    // Fila 2: Depth (centrado)
    {
        const int depthW = b.getWidth() / 2;
        auto depthArea = juce::Rectangle<int>(
            b.getX() + (b.getWidth() - depthW) / 2,
            b.getY(), depthW, b.getHeight());
        depthLabel.setBounds(depthArea.removeFromBottom(labelH));
        depthSlider.setBounds(depthArea.reduced(2));
    }
}

void ModEnvComponent::setSliderParams(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
    s.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xff8effc1));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a50));
    s.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
    s.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    s.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    addAndMakeVisible(s);
}

void ModEnvComponent::setLabelParams(juce::Label& l, const juce::String& t)
{
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(11.5f, juce::Font::bold));
    l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(l);
}
