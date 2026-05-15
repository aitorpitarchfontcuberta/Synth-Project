/*
  ==============================================================================

    VelocityCurveVisualizer.cpp

  ==============================================================================
*/

#include "VelocityCurveVisualizer.h"

VelocityCurveVisualizer::VelocityCurveVisualizer(juce::AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    startTimerHz(20);
}

VelocityCurveVisualizer::~VelocityCurveVisualizer()
{
    stopTimer();
}

void VelocityCurveVisualizer::timerCallback()
{
    const float v = apvtsRef.getRawParameterValue("VEL_SENS")->load();
    if (v != lastVelSens) { lastVelSens = v; repaint(); }
}

void VelocityCurveVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    const float velSens = apvtsRef.getRawParameterValue("VEL_SENS")->load();

    const float pad = 8.0f;
    const float x0 = bounds.getX() + pad;
    const float x1 = bounds.getRight() - pad;
    const float y0 = bounds.getY() + pad;
    const float y1 = bounds.getBottom() - pad;

    // Grilla diagonal de referencia (linea Y=X tenue)
    g.setColour(juce::Colour(0xff242438));
    g.drawLine(x0, y1, x1, y0, 1.0f);

    // Linea de "vel sens = 0" (constante) - tenue
    g.setColour(juce::Colour(0xff242438));
    g.drawHorizontalLine((int)y0, x0, x1);

    // Curva actual: amp = (1 - velSens) + velSens * vel
    juce::Path curve;
    const int N = 64;
    bool first = true;
    for (int i = 0; i <= N; ++i)
    {
        const float vel = (float)i / (float)N;
        const float amp = (1.0f - velSens) + velSens * vel;
        const float x = juce::jmap(vel, 0.0f, 1.0f, x0, x1);
        const float y = juce::jmap(amp, 0.0f, 1.0f, y1, y0);
        if (first) { curve.startNewSubPath(x, y); first = false; }
        else       { curve.lineTo(x, y); }
    }
    g.setColour(juce::Colour(0xfffff066));
    g.strokePath(curve, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved));

    // Etiquetas
    g.setColour(juce::Colours::lightgrey);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("VELOCITY",
               bounds.toNearestInt().reduced(6, 3),
               juce::Justification::topLeft);
    g.setColour(juce::Colour(0xfffff066).withAlpha(0.7f));
    g.drawText("sens " + juce::String(velSens, 2),
               bounds.toNearestInt().reduced(6, 3),
               juce::Justification::topRight);
    g.setColour(juce::Colours::darkgrey);
    g.setFont(juce::Font(8.0f));
    g.drawText("soft",     juce::Rectangle<int>((int)x0 - 4, (int)y1 - 6, 40, 14),
               juce::Justification::topLeft);
    g.drawText("hard",     juce::Rectangle<int>((int)x1 - 40, (int)y1 - 6, 40, 14),
               juce::Justification::topRight);
}
