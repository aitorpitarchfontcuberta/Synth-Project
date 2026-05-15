/*
  ==============================================================================

    FilterResponseVisualizer.cpp

  ==============================================================================
*/

#include "FilterResponseVisualizer.h"
#include "../Data/FilterData.h"

FilterResponseVisualizer::FilterResponseVisualizer(juce::AudioProcessorValueTreeState& apvts,
                                                   const ScopeBuffer& scopeBuffer)
    : apvtsRef(apvts), bufferRef(scopeBuffer)
{
    startTimerHz(20);
}

FilterResponseVisualizer::~FilterResponseVisualizer()
{
    stopTimer();
}

void FilterResponseVisualizer::timerCallback()
{
    repaint();
}

void FilterResponseVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    // Fondo
    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    const float pad = 8.0f;
    const float x0 = bounds.getX() + pad;
    const float x1 = bounds.getRight() - pad;
    const float y0 = bounds.getY() + pad;
    const float y1 = bounds.getBottom() - pad;
    const float plotW = x1 - x0;
    const float plotH = y1 - y0;

    const double sr = bufferRef.sampleRate.load(std::memory_order_relaxed);

    const float fMin = 20.0f;
    const float fMax = (float) juce::jmin(20000.0, sr * 0.5);
    const float dBMin = -36.0f;
    const float dBMax = 18.0f;

    // Grilla vertical (frecuencias clave)
    g.setColour(juce::Colour(0xff242438));
    auto xForFreq = [&](float freq) {
        return x0 + (std::log10(freq / fMin) / std::log10(fMax / fMin)) * plotW;
    };
    for (float f : { 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f })
    {
        if (f < fMin || f > fMax) continue;
        g.drawVerticalLine((int) xForFreq(f), y0, y1);
    }

    // Linea de 0 dB
    auto yForDb = [&](float db) {
        return juce::jmap(db, dBMin, dBMax, y1, y0);
    };
    g.setColour(juce::Colour(0xff2a2a40));
    g.drawHorizontalLine((int) yForDb(0.0f), x0, x1);

    // Lee parametros actuales del filtro
    const int   type    = (int)apvtsRef.getRawParameterValue("FILTER_TYPE")->load();
    const float cutoff  = apvtsRef.getRawParameterValue("FILTER_CUTOFF")->load();
    const float res     = apvtsRef.getRawParameterValue("FILTER_RES")->load();

    // Dibuja la curva de respuesta
    juce::Path response;
    bool first = true;
    const int numPoints = 256;
    for (int i = 0; i <= numPoints; ++i)
    {
        const float t = (float) i / (float) numPoints;
        const float freq = fMin * std::pow(fMax / fMin, t);
        const double magLinear = FilterData::getMagnitudeAtFrequency(type, cutoff, res, freq, sr);
        const float magDb = (float) juce::Decibels::gainToDecibels(magLinear + 1.0e-9);
        const float magClamped = juce::jlimit(dBMin, dBMax, magDb);
        const float px = juce::jmap(t, 0.0f, 1.0f, x0, x1);
        const float py = yForDb(magClamped);
        if (first) { response.startNewSubPath(px, py); first = false; }
        else       { response.lineTo(px, py); }
    }

    // Relleno suave debajo de la curva
    juce::Path fillPath = response;
    fillPath.lineTo(x1, y1);
    fillPath.lineTo(x0, y1);
    fillPath.closeSubPath();

    g.setColour(juce::Colour(0xff8effc1).withAlpha(0.18f));
    g.fillPath(fillPath);

    g.setColour(juce::Colour(0xff8effc1));
    g.strokePath(response, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved));

    // Marcador vertical en el cutoff
    {
        const float fx = xForFreq(juce::jlimit(fMin, fMax, cutoff));
        g.setColour(juce::Colour(0xff8effc1).withAlpha(0.55f));
        g.drawVerticalLine((int) fx, y0, y1);

        g.setColour(juce::Colours::white);
        g.fillEllipse(fx - 3.0f, yForDb(0.0f) - 3.0f, 6.0f, 6.0f);
    }

    // Etiquetas
    g.setColour(juce::Colours::darkgrey);
    g.setFont(juce::Font(8.5f));
    auto drawFreqLabel = [&](float freq, const char* label)
    {
        if (freq < fMin || freq > fMax) return;
        g.drawText(label,
                   juce::Rectangle<int>((int) xForFreq(freq) - 18, (int) y1 - 12, 36, 12),
                   juce::Justification::centred);
    };
    drawFreqLabel(100.0f,   "100");
    drawFreqLabel(1000.0f,  "1k");
    drawFreqLabel(10000.0f, "10k");

    // Esquina con info
    const char* typeNames[] = { "LP", "HP", "BP", "Notch" };
    g.setColour(juce::Colour(0xff8effc1));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("FILTER " + juce::String(typeNames[juce::jlimit(0, 3, type)]),
               bounds.toNearestInt().reduced(8, 4),
               juce::Justification::topLeft);

    g.setColour(juce::Colour(0xff8effc1).withAlpha(0.7f));
    g.setFont(juce::Font(9.5f));
    juce::String cutoffStr = cutoff >= 1000.0f
        ? juce::String(cutoff / 1000.0f, 2) + " kHz"
        : juce::String((int) cutoff) + " Hz";
    g.drawText("cutoff " + cutoffStr,
               bounds.toNearestInt().reduced(8, 4),
               juce::Justification::topRight);
}
