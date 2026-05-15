/*
  ==============================================================================

    GlideVisualizer.cpp

  ==============================================================================
*/

#include "GlideVisualizer.h"

GlideVisualizer::GlideVisualizer(juce::AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    startTimerHz(20);
}

GlideVisualizer::~GlideVisualizer()
{
    stopTimer();
}

void GlideVisualizer::timerCallback()
{
    const float g = apvtsRef.getRawParameterValue("GLIDE")->load();
    if (g != lastGlide) { lastGlide = g; repaint(); }
}

void GlideVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    const float glide = apvtsRef.getRawParameterValue("GLIDE")->load();

    const float pad = 8.0f;
    const float x0 = bounds.getX() + pad;
    const float x1 = bounds.getRight() - pad;
    const float y0 = bounds.getY() + pad;
    const float y1 = bounds.getBottom() - pad;

    // Asumimos una "ventana temporal" total de 2.5 segundos para representar
    // la transicion. La nueva nota se dispara a t = 0.3 s (margen izquierdo).
    const float windowSeconds = 2.5f;
    const float noteOnTime = 0.3f;

    // Lineas guia: nivel inicial (alto) y nivel final (bajo)
    const float yHigh = juce::jmap(0.75f, 0.0f, 1.0f, y1, y0);
    const float yLow  = juce::jmap(0.25f, 0.0f, 1.0f, y1, y0);

    g.setColour(juce::Colour(0xff242438));
    g.drawHorizontalLine((int)yHigh, x0, x1);
    g.drawHorizontalLine((int)yLow,  x0, x1);

    // Path: nota 1 plana, luego rampa hacia nota 2 segun el glide
    const float xNoteOn = juce::jmap(noteOnTime / windowSeconds, 0.0f, 1.0f, x0, x1);
    const float xRampEnd = juce::jmap(juce::jlimit(0.0f, windowSeconds - noteOnTime, glide)
                                      / windowSeconds + noteOnTime / windowSeconds,
                                      0.0f, 1.0f, x0, x1);

    juce::Path glidePath;
    glidePath.startNewSubPath(x0,        yHigh);
    glidePath.lineTo(xNoteOn,           yHigh);
    if (glide < 0.001f)
    {
        // Salto instantaneo
        glidePath.lineTo(xNoteOn, yLow);
        glidePath.lineTo(x1,       yLow);
    }
    else
    {
        glidePath.lineTo(xRampEnd, yLow);
        glidePath.lineTo(x1,        yLow);
    }

    g.setColour(juce::Colour(0xfffff066));
    g.strokePath(glidePath, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved));

    // Marcadores de las dos "notas"
    g.setColour(juce::Colours::white);
    g.fillEllipse(xNoteOn - 3.0f, yHigh - 3.0f, 6.0f, 6.0f);
    g.fillEllipse((glide < 0.001f ? xNoteOn : xRampEnd) - 3.0f, yLow - 3.0f, 6.0f, 6.0f);

    // Marcador vertical del momento de note-on
    g.setColour(juce::Colour(0xff7a9dff).withAlpha(0.4f));
    g.drawVerticalLine((int)xNoteOn, y0, y1);

    // Etiquetas
    g.setColour(juce::Colours::lightgrey);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("GLIDE",
               bounds.toNearestInt().reduced(6, 3),
               juce::Justification::topLeft);
    g.setColour(juce::Colour(0xfffff066).withAlpha(0.7f));
    g.drawText(juce::String(glide, 2) + " s",
               bounds.toNearestInt().reduced(6, 3),
               juce::Justification::topRight);
}
