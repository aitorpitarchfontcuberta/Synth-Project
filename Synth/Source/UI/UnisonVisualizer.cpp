/*
  ==============================================================================

    UnisonVisualizer.cpp

  ==============================================================================
*/

#include "UnisonVisualizer.h"

UnisonVisualizer::UnisonVisualizer(juce::AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    startTimerHz(20);
}

UnisonVisualizer::~UnisonVisualizer()
{
    stopTimer();
}

void UnisonVisualizer::timerCallback()
{
    const int   voices  = (int)apvtsRef.getRawParameterValue("UNISON_VOICES")->load();
    const float detune  = apvtsRef.getRawParameterValue("UNISON_DETUNE")->load();
    const float spread  = apvtsRef.getRawParameterValue("UNISON_SPREAD")->load();
    if (voices != lastVoices || detune != lastDetune || spread != lastSpread)
    {
        lastVoices = voices; lastDetune = detune; lastSpread = spread;
        repaint();
    }
}

void UnisonVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    const int voices = (int)apvtsRef.getRawParameterValue("UNISON_VOICES")->load();
    const float detune = apvtsRef.getRawParameterValue("UNISON_DETUNE")->load();
    const float spread = apvtsRef.getRawParameterValue("UNISON_SPREAD")->load();

    const float midX = bounds.getCentreX();
    const float midY = bounds.getCentreY();

    // Linea central
    g.setColour(juce::Colour(0xff242438));
    g.drawVerticalLine((int)midX, bounds.getY() + 4, bounds.getBottom() - 4);

    const float horizontalRange = bounds.getWidth() * 0.4f;
    const float verticalRange   = bounds.getHeight() * 0.3f;

    // Dibujamos un punto por voz, distribuidos segun detune en X y spread en Y
    for (int u = 0; u < voices; ++u)
    {
        const float t = (voices <= 1) ? 0.0f
                                      : ((float)u / (float)(voices - 1) * 2.0f - 1.0f);
        const float detRatio = detune / 50.0f; // normalizar a [0,1]
        const float x = midX + t * detRatio * horizontalRange;
        const float y = midY + t * spread * verticalRange;

        // Color con alpha segun cercania al centro (visualmente agrupado)
        g.setColour(juce::Colour(0xfffff066).withAlpha(0.85f));
        g.fillEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f);
        g.setColour(juce::Colours::white);
        g.drawEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f, 1.0f);
    }

    // Texto pequeno con el numero de voces
    g.setColour(juce::Colours::lightgrey);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText(juce::String(voices) + " voice" + (voices > 1 ? "s" : ""),
               bounds.toNearestInt().removeFromBottom(14),
               juce::Justification::centred);
}
