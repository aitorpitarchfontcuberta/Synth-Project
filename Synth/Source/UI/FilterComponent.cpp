/*
  ==============================================================================

    FilterComponent.cpp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterComponent.h"

FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& apvts,
                                 const ScopeBuffer& scopeBuffer)
{
    titleLabel.setText("FILTER", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8effc1));
    addAndMakeVisible(titleLabel);

    juce::StringArray types { "LP","HP","BP","Notch" };
    typeBox.addItemList(types, 1);
    typeBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(typeBox);
    setLabelParams(typeLabel, "Type");

    setSliderParams(cutoffSlider);
    setSliderParams(resSlider);
    setSliderParams(driveSlider);
    setSliderParams(keyTrackSlider);

    cutoffSlider.setTextValueSuffix(" Hz");

    setLabelParams(cutoffLabel,   "Cutoff");
    setLabelParams(resLabel,      "Reso");
    setLabelParams(driveLabel,    "Drive");
    setLabelParams(keyTrackLabel, "Key Track");

    typeA     = std::make_unique<CA>(apvts, "FILTER_TYPE",     typeBox);
    cutoffA   = std::make_unique<SA>(apvts, "FILTER_CUTOFF",   cutoffSlider);
    resA      = std::make_unique<SA>(apvts, "FILTER_RES",      resSlider);
    driveA    = std::make_unique<SA>(apvts, "FILTER_DRIVE",    driveSlider);
    keyTrackA = std::make_unique<SA>(apvts, "FILTER_KEYTRACK", keyTrackSlider);

    responseVis = std::make_unique<FilterResponseVisualizer>(apvts, scopeBuffer);
    addAndMakeVisible(*responseVis);
}

FilterComponent::~FilterComponent() {}

void FilterComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    juce::ColourGradient grad(juce::Colour(0xff2a2a3a), bounds.getTopLeft(),
                              juce::Colour(0xff1a1a25), bounds.getBottomRight(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(juce::Colour(0xff4a4a6a));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void FilterComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Fila superior: titulo a la izq + combo type a la derecha
    auto topRow = bounds.removeFromTop(24);
    titleLabel.setBounds(topRow.removeFromLeft(80));
    typeLabel.setBounds(topRow.removeFromLeft(40));
    typeBox.setBounds(topRow);

    bounds.removeFromTop(6);

    // Visualizador grande de la respuesta del filtro (la pieza didactica)
    auto visArea = bounds.removeFromTop((int)(bounds.getHeight() * 0.6f));
    if (responseVis) responseVis->setBounds(visArea);

    bounds.removeFromTop(10);

    // Knobs: Cutoff, Reso, Drive, Key Track (4 columnas)
    const int labelH = 18;
    const int n = 4;
    const int w = bounds.getWidth() / n;
    auto layout = [&](juce::Slider& s, juce::Label& l, int i)
    {
        auto a = juce::Rectangle<int>(bounds.getX() + i * w, bounds.getY(), w, bounds.getHeight());
        l.setBounds(a.removeFromBottom(labelH));
        s.setBounds(a.reduced(2));
    };
    layout(cutoffSlider,   cutoffLabel,   0);
    layout(resSlider,      resLabel,      1);
    layout(driveSlider,    driveLabel,    2);
    layout(keyTrackSlider, keyTrackLabel, 3);
}

void FilterComponent::setSliderParams(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xff8effc1));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a50));
    s.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
    s.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    s.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    addAndMakeVisible(s);
}

void FilterComponent::setLabelParams(juce::Label& l, const juce::String& text)
{
    l.setText(text, juce::dontSendNotification);
    l.setFont(juce::Font(11.5f, juce::Font::bold));
    l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(l);
}
