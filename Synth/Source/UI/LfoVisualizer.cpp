/*
  ==============================================================================

    LfoVisualizer.cpp

  ==============================================================================
*/

#include "LfoVisualizer.h"

LfoVisualizer::LfoVisualizer(juce::AudioProcessorValueTreeState& apvts,
                             const juce::String& waveID,
                             const juce::String& rateID,
                             const juce::String& depthID,
                             juce::Colour curveColour)
    : apvtsRef(apvts), wID(waveID), rID(rateID), dID(depthID), colour(curveColour)
{
    lastTimeMs = juce::Time::currentTimeMillis();
    startTimerHz(30);
}

LfoVisualizer::~LfoVisualizer()
{
    stopTimer();
}

float LfoVisualizer::renderWave(int waveType, float phase)
{
    phase -= std::floor(phase);
    switch (waveType)
    {
        case 0: return std::sin(phase * juce::MathConstants<float>::twoPi);
        case 1: return phase < 0.5f ? (-1.0f + 4.0f * phase)
                                    : ( 3.0f - 4.0f * phase);
        case 2: return 2.0f * phase - 1.0f;
        case 3: return phase < 0.5f ? 1.0f : -1.0f;
        case 4: // S&H decorativo (escalones aleatorios estaticos)
        {
            static const float steps[8] = { 0.6f, -0.3f, 0.9f, -0.7f, 0.1f, -0.5f, 0.8f, -0.2f };
            const int idx = (int)(phase * 8) & 7;
            return steps[idx];
        }
    }
    return 0.0f;
}

void LfoVisualizer::timerCallback()
{
    // Avanzar la fase animada segun el tiempo real y el rate actual
    const float rate = apvtsRef.getRawParameterValue(rID)->load();
    const auto  now  = juce::Time::currentTimeMillis();
    const float dt   = (float)(now - lastTimeMs) / 1000.0f;
    lastTimeMs = now;

    // Para que la animacion sea didactica pero no mareante con rates altos,
    // limitamos visualmente la velocidad del playhead a 4 ciclos/segundo aprox.
    const float visualRate = juce::jlimit(0.01f, 4.0f, rate);
    animPhase += visualRate * dt;
    animPhase -= std::floor(animPhase);

    repaint();
}

void LfoVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    const float midY = bounds.getCentreY();
    g.setColour(juce::Colour(0xff242438));
    g.drawHorizontalLine((int)midY, bounds.getX() + 4, bounds.getRight() - 4);

    const int   waveType = (int)apvtsRef.getRawParameterValue(wID)->load();
    const float rate     = apvtsRef.getRawParameterValue(rID)->load();
    const float depth    = apvtsRef.getRawParameterValue(dID)->load();

    // Numero de ciclos visibles: mapeo logaritmico para que rates bajos
    // tambien sean visibles. Rate 0.01-30 Hz -> ~0.5 a 8 ciclos visibles.
    const float numCycles = juce::jlimit(0.5f, 8.0f, std::log10(rate * 10.0f + 1.0f) * 3.5f);

    const float padding = 6.0f;
    const float x0 = bounds.getX() + padding;
    const float x1 = bounds.getRight() - padding;
    const float plotW = x1 - x0;
    const float plotH = bounds.getHeight() - padding * 2.0f;
    const float amplitudeY = plotH * 0.45f * juce::jlimit(0.0f, 1.0f, depth);

    // Dibujar la onda
    const int numSamples = juce::jlimit(120, 720, (int)(60.0f * numCycles));

    juce::Path wavePath;
    bool first = true;
    for (int i = 0; i <= numSamples; ++i)
    {
        const float t = (float)i / (float)numSamples * numCycles;
        const float v = renderWave(waveType, t);
        const float x = juce::jmap((float)i / (float)numSamples, 0.0f, 1.0f, x0, x1);
        const float y = midY - v * amplitudeY;
        if (first) { wavePath.startNewSubPath(x, y); first = false; }
        else        { wavePath.lineTo(x, y); }
    }

    // Si depth == 0, dibujamos atenuado para indicar que el LFO no tiene efecto
    g.setColour(colour.withAlpha(depth > 0.001f ? 1.0f : 0.35f));
    g.strokePath(wavePath, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved));

    // Playhead: linea vertical que se mueve a lo largo de la onda
    {
        const float playheadFracInVisible = animPhase; // [0, 1)
        const float px = juce::jmap(playheadFracInVisible, 0.0f, 1.0f, x0, x1);

        g.setColour(juce::Colours::white.withAlpha(0.55f));
        g.drawVerticalLine((int)px, bounds.getY() + 4, bounds.getBottom() - 4);

        // Punto que sigue la onda en la posicion del playhead
        const float vAtPlayhead = renderWave(waveType, playheadFracInVisible * numCycles);
        const float py = midY - vAtPlayhead * amplitudeY;
        g.setColour(juce::Colours::white);
        g.fillEllipse(px - 3.0f, py - 3.0f, 6.0f, 6.0f);
    }

    // Etiquetas informativas en las esquinas
    g.setColour(colour.withAlpha(0.55f));
    g.setFont(juce::Font(9.0f));
    g.drawText(juce::String(rate, 2) + " Hz",
               bounds.toNearestInt().reduced(6, 3),
               juce::Justification::topLeft);
    g.drawText("depth " + juce::String(depth, 2),
               bounds.toNearestInt().reduced(6, 3),
               juce::Justification::topRight);
}
