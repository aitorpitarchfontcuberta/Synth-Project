/*
  ==============================================================================

    WaveformVisualizer.cpp

  ==============================================================================
*/

#include "WaveformVisualizer.h"

WaveformVisualizer::WaveformVisualizer(juce::AudioProcessorValueTreeState& apvts,
                                       const juce::String& waveID,
                                       const juce::String& pulseWidthID,
                                       juce::Colour curveColour,
                                       Mode m,
                                       const juce::String& octaveID,
                                       const juce::String& semiID,
                                       const juce::String& fineID)
    : apvtsRef(apvts), wID(waveID), pwID(pulseWidthID),
      octID(octaveID), semID(semiID), finID(fineID),
      colour(curveColour), mode(m)
{
    startTimerHz(20);
}

WaveformVisualizer::~WaveformVisualizer()
{
    stopTimer();
}

void WaveformVisualizer::timerCallback()
{
    const int   w  = (int)apvtsRef.getRawParameterValue(wID)->load();
    const float pw = pwID.isNotEmpty() ? apvtsRef.getRawParameterValue(pwID)->load() : 0.5f;

    const int   oct  = octID.isNotEmpty() ? (int)apvtsRef.getRawParameterValue(octID)->load() : 0;
    const int   sem  = semID.isNotEmpty() ? (int)apvtsRef.getRawParameterValue(semID)->load() : 0;
    const float fine = finID.isNotEmpty() ? apvtsRef.getRawParameterValue(finID)->load()      : 0.0f;

    if (w != lastWave || pw != lastPW
        || oct != lastOctave || sem != lastSemi || fine != lastFine)
    {
        lastWave = w;     lastPW = pw;
        lastOctave = oct; lastSemi = sem; lastFine = fine;
        repaint();
    }
}

void WaveformVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    const float midY = bounds.getCentreY();
    g.setColour(juce::Colour(0xff242438));
    g.drawHorizontalLine((int)midY, bounds.getX() + 4, bounds.getRight() - 4);

    const int waveType = (int)apvtsRef.getRawParameterValue(wID)->load();
    const float pw = pwID.isNotEmpty() ? apvtsRef.getRawParameterValue(pwID)->load() : 0.5f;

    // Calculo del numero de ciclos visibles segun el pitch (solo si se han pasado los IDs)
    float numCycles = 2.0f; // por defecto, 2 ciclos
    if (octID.isNotEmpty() || semID.isNotEmpty() || finID.isNotEmpty())
    {
        const int   oct  = octID.isNotEmpty() ? (int)apvtsRef.getRawParameterValue(octID)->load() : 0;
        const int   sem  = semID.isNotEmpty() ? (int)apvtsRef.getRawParameterValue(semID)->load() : 0;
        const float fine = finID.isNotEmpty() ? apvtsRef.getRawParameterValue(finID)->load()      : 0.0f;

        const float semis = (float)(oct * 12 + sem) + fine / 100.0f;
        const float factor = std::pow(2.0f, semis / 12.0f);
        numCycles = juce::jlimit(0.25f, 8.0f, 2.0f * factor);
    }

    const float padding = 6.0f;
    const float x0 = bounds.getX() + padding;
    const float x1 = bounds.getRight() - padding;
    const float y0 = bounds.getY() + padding;
    const float y1 = bounds.getBottom() - padding;
    const float amplitudeY = (y1 - y0) * 0.45f;

    // Resolucion adaptativa: mas ciclos = mas puntos
    const int   numSamples = juce::jlimit(120, 720, (int)(60.0f * numCycles));

    auto valueFor = [&](float t /*[0, numCycles)*/) -> float
    {
        const float ph = t - std::floor(t);

        if (mode == Mode::Oscillator)
        {
            switch (waveType)
            {
                case 0: return std::sin(ph * juce::MathConstants<float>::twoPi);
                case 1: return ph < 0.5f ? (-1.0f + 4.0f * ph) : (3.0f - 4.0f * ph);
                case 2: return 2.0f * ph - 1.0f;
                case 3: return ph < 0.5f ? 1.0f : -1.0f;
                case 4: return ph < pw   ? 1.0f : -1.0f;
                case 5: return std::sin(ph * 47.0f) * std::cos(ph * 73.0f);
            }
        }
        else // LFO
        {
            switch (waveType)
            {
                case 0: return std::sin(ph * juce::MathConstants<float>::twoPi);
                case 1: return ph < 0.5f ? (-1.0f + 4.0f * ph) : (3.0f - 4.0f * ph);
                case 2: return 2.0f * ph - 1.0f;
                case 3: return ph < 0.5f ? 1.0f : -1.0f;
                case 4:
                {
                    static const float steps[8] = { 0.6f, -0.3f, 0.9f, -0.7f, 0.1f, -0.5f, 0.8f, -0.2f };
                    const int idx = (int)(ph * 8) & 7;
                    return steps[idx];
                }
            }
        }
        return 0.0f;
    };

    juce::Path path;
    bool first = true;
    for (int i = 0; i <= numSamples; ++i)
    {
        const float t = (float)i / (float)numSamples * numCycles;
        const float v = valueFor(t);
        const float x = juce::jmap((float)i / (float)numSamples, 0.0f, 1.0f, x0, x1);
        const float y = midY - v * amplitudeY;
        if (first) { path.startNewSubPath(x, y); first = false; }
        else       { path.lineTo(x, y); }
    }

    g.setColour(colour);
    g.strokePath(path, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved));

    // Pequena etiqueta en la esquina con el numero de ciclos visibles (informativo)
    if (octID.isNotEmpty() || semID.isNotEmpty() || finID.isNotEmpty())
    {
        g.setColour(colour.withAlpha(0.5f));
        g.setFont(juce::Font(9.0f));
        g.drawText(juce::String(numCycles, 2) + " cycles",
                   bounds.toNearestInt().reduced(6, 3),
                   juce::Justification::topRight);
    }
}
