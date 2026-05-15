/*
  ==============================================================================

    OscilloscopeComponent.cpp

  ==============================================================================
*/

#include "OscilloscopeComponent.h"

OscilloscopeComponent::OscilloscopeComponent(const ScopeBuffer& buffer)
    : bufferRef(buffer)
{
    startTimerHz(30);
}

OscilloscopeComponent::~OscilloscopeComponent()
{
    stopTimer();
}

void OscilloscopeComponent::timerCallback()
{
    // Copia el snapshot mas reciente del buffer compartido
    bufferRef.copyLastN(drawData.data(), kDrawSamples);

    // Calcula el pico para mostrar un nivel visual
    float maxAbs = 0.0f;
    for (float s : drawData)
        maxAbs = std::max(maxAbs, std::abs(s));
    peakLevel = peakLevel * 0.7f + maxAbs * 0.3f; // smoothing visual

    repaint();
}

void OscilloscopeComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    // Fondo
    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    // Grilla
    const float midY = bounds.getCentreY();
    g.setColour(juce::Colour(0xff1c1c2a));
    for (int i = 1; i < 4; ++i)
    {
        float y = bounds.getY() + bounds.getHeight() * (i / 4.0f);
        g.drawHorizontalLine((int)y, bounds.getX() + 6, bounds.getRight() - 6);
    }
    g.setColour(juce::Colour(0xff2a2a40));
    g.drawHorizontalLine((int)midY, bounds.getX() + 6, bounds.getRight() - 6);

    // Etiqueta
    g.setColour(juce::Colour(0xff7a9dff));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("OUTPUT", bounds.toNearestInt().reduced(8, 4),
               juce::Justification::topLeft);

    // Nivel (barra a la derecha)
    {
        auto meterArea = juce::Rectangle<float>(
            bounds.getRight() - 14.0f, bounds.getY() + 6.0f,
            6.0f, bounds.getHeight() - 12.0f);
        g.setColour(juce::Colour(0xff1c1c2a));
        g.fillRoundedRectangle(meterArea, 2.0f);

        const float lvl = juce::jlimit(0.0f, 1.0f, peakLevel);
        auto lvlArea = meterArea.removeFromBottom(meterArea.getHeight() * lvl);
        g.setColour(lvl > 0.95f ? juce::Colours::red.withAlpha(0.9f)
                                : juce::Colour(0xff7a9dff).withAlpha(0.85f));
        g.fillRoundedRectangle(lvlArea, 2.0f);
    }

    // Trigger: buscar el primer cruce por cero ascendente en la primera mitad
    int startIdx = 0;
    for (int i = 1; i < kDrawSamples / 2; ++i)
    {
        if (drawData[(size_t)(i - 1)] <= 0.0f && drawData[(size_t)i] > 0.0f)
        {
            startIdx = i;
            break;
        }
    }
    const int numToDraw = kDrawSamples - startIdx;

    // Calculamos un factor de escalado adaptativo: si la senial es muy debil,
    // ampliamos la traza para que se vea algo; si esta saturada, la comprimimos.
    const float visualGain = (peakLevel > 0.01f)
                                 ? juce::jlimit(0.5f, 4.0f, 0.8f / peakLevel)
                                 : 1.0f;

    const float plotX0 = bounds.getX() + 6.0f;
    const float plotX1 = bounds.getRight() - 20.0f;
    const float plotW  = plotX1 - plotX0;
    const float amp    = bounds.getHeight() * 0.45f;

    juce::Path trace;
    bool first = true;
    for (int i = 0; i < numToDraw; ++i)
    {
        const float x = plotX0 + (float)i / (float)(numToDraw - 1) * plotW;
        const float v = juce::jlimit(-1.0f, 1.0f, drawData[(size_t)(startIdx + i)] * visualGain);
        const float y = midY - v * amp;
        if (first) { trace.startNewSubPath(x, y); first = false; }
        else       { trace.lineTo(x, y); }
    }

    g.setColour(juce::Colour(0xff7a9dff));
    g.strokePath(trace, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved));
}
