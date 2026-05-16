/*
  ==============================================================================

    MasterComponent.cpp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MasterComponent.h"

MasterComponent::MasterComponent(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setText("MASTER", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff7a9dff));
    addAndMakeVisible(titleLabel);

    juce::StringArray modes { "Poly","Mono","Legato" };
    voiceModeBox.addItemList(modes, 1);
    voiceModeBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(voiceModeBox);
    setLabelParams(voiceModeLabel, "Voice");

    // --- Sliders de fila inferior (con TextBoxBelow) ---
    setSliderParamsCompact(gainSlider);
    setSliderParamsCompact(unisonSlider);
    setSliderParamsCompact(detuneSlider);
    setSliderParamsCompact(spreadSlider);
    detuneSlider.setTextValueSuffix(" ct");

    // --- Sliders laterales (Vel y Glide) con TextBoxLeft ---
    setSliderParamsSide(velSlider);
    setSliderParamsSide(glideSlider);
    glideSlider.setTextValueSuffix(" s");

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
    auto b = getLocalBounds().reduced(10);

    // Cabecera: titulo + Voice combo
    auto headRow = b.removeFromTop(24);
    titleLabel.setBounds(headRow.removeFromLeft(80));
    voiceModeBox.setBounds(headRow.removeFromRight(headRow.getWidth() / 2 + 20));
    voiceModeLabel.setBounds(headRow.removeFromRight(40));

    b.removeFromTop(6);

    // Filas Velocity y Glide: visualizador estrecho + textos+knob mas grandes a la derecha.
    const int sideRowH    = 70;   // un poco mas alto para que el knob respire
    const int textboxW    = 50;   // ancho del textbox a la izquierda del knob
    const int knobW       = 56;   // ancho del area del knob rotativo
    const int sideTotalW  = textboxW + knobW;
    const int labelH      = 14;
    const int visGap      = 6;

    auto layoutSideRow = [&](juce::Component& visualizer,
                             juce::Slider&    slider,
                             juce::Label&     label)
    {
        auto row = b.removeFromTop(sideRowH);

        // Reservar a la derecha el bloque slider+textbox
        auto sliderTotalArea = row.removeFromRight(sideTotalW);

        // Etiqueta encima del textbox (los primeros textboxW px)
        auto labelArea = sliderTotalArea.removeFromTop(labelH);
        label.setBounds(labelArea.removeFromLeft(textboxW));

        // Slider ocupa el resto (textbox a la izquierda, knob a la derecha)
        slider.setBounds(sliderTotalArea);

        // Visualizador ocupa lo restante a la izquierda
        row.removeFromRight(visGap);
        visualizer.setBounds(row);

        b.removeFromTop(6);
    };

    if (velVis)   layoutSideRow(*velVis,   velSlider,   velLabel);
    if (glideVis) layoutSideRow(*glideVis, glideSlider, glideLabel);

    // Unison visualizer (ancho completo, sin knob lateral)
    if (unisonVis)
    {
        unisonVis->setBounds(b.removeFromTop(60));
        b.removeFromTop(10);
    }

    // Fila inferior con 4 knobs (Detune, Spread, Unison, Gain).
    // Ocupa el espacio sobrante.
    auto bottomRow = b;

    const int n = 4;
    const int w = bottomRow.getWidth() / n;
    auto layoutKnob = [&](juce::Slider& s, juce::Label& l, int i)
    {
        auto a = juce::Rectangle<int>(bottomRow.getX() + i * w,
                                      bottomRow.getY(),
                                      w, bottomRow.getHeight());
        l.setBounds(a.removeFromBottom(labelH));
        s.setBounds(a.reduced(2));
    };
    layoutKnob(detuneSlider, detuneLabel, 0);
    layoutKnob(spreadSlider, spreadLabel, 1);
    layoutKnob(unisonSlider, unisonLabel, 2);
    layoutKnob(gainSlider,   gainLabel,   3);
}

void MasterComponent::setSliderParamsCompact(juce::Slider& s)
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

void MasterComponent::setSliderParamsSide(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 22);
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
    l.setFont(juce::Font(11.0f, juce::Font::bold));
    l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(l);
}
