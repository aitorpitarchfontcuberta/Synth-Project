/*
  ==============================================================================

    MixBarVisualizer.cpp

  ==============================================================================
*/

#include "MixBarVisualizer.h"

MixBarVisualizer::MixBarVisualizer(juce::AudioProcessorValueTreeState& apvts,
                                   const juce::String& mixParamID)
    : apvtsRef(apvts), mixID(mixParamID)
{
    startTimerHz(20);
}

MixBarVisualizer::~MixBarVisualizer()
{
    stopTimer();
}

void MixBarVisualizer::timerCallback()
{
    const float mix = apvtsRef.getRawParameterValue(mixID)->load();
    if (mix != lastMix)
    {
        lastMix = mix;
        repaint();
    }
}

void MixBarVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    const float mix = apvtsRef.getRawParameterValue(mixID)->load();
    auto inner = bounds.reduced(4.0f);

    // Bloque izquierdo (OSC1) y derecho (OSC2)
    const float splitX = inner.getX() + inner.getWidth() * (1.0f - mix);

    auto leftR  = juce::Rectangle<float>(inner.getX(), inner.getY(),
                                         splitX - inner.getX(), inner.getHeight());
    auto rightR = juce::Rectangle<float>(splitX, inner.getY(),
                                         inner.getRight() - splitX, inner.getHeight());

    g.setColour(juce::Colour(0xff7a9dff).withAlpha(0.7f));
    g.fillRoundedRectangle(leftR, 2.0f);
    g.setColour(juce::Colour(0xffffaa55).withAlpha(0.7f));
    g.fillRoundedRectangle(rightR, 2.0f);

    // Etiquetas
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("OSC1", leftR.toNearestInt(),  juce::Justification::centred);
    g.drawText("OSC2", rightR.toNearestInt(), juce::Justification::centred);
}
