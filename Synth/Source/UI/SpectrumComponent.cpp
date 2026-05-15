/*
  ==============================================================================

    SpectrumComponent.cpp

  ==============================================================================
*/

#include "SpectrumComponent.h"
#include "../Data/FilterData.h"

SpectrumComponent::SpectrumComponent(const ScopeBuffer& buffer,
                                     juce::AudioProcessorValueTreeState* apvtsForFilterOverlay)
    : bufferRef(buffer),
      apvtsRef(apvtsForFilterOverlay),
      fft(kFftOrder),
      window((size_t) kFftSize, juce::dsp::WindowingFunction<float>::hann)
{
    startTimerHz(30);
}

SpectrumComponent::~SpectrumComponent()
{
    stopTimer();
}

void SpectrumComponent::timerCallback()
{
    // Toma las ultimas kFftSize muestras del buffer
    bufferRef.copyLastN(fftData.data(), kFftSize);

    // Aplica ventana Hann para reducir spectral leakage
    window.multiplyWithWindowingTable(fftData.data(), (size_t) kFftSize);

    // FFT in-place: transforma a magnitudes
    fft.performFrequencyOnlyForwardTransform(fftData.data());

    // Copia y suaviza para que el espectro no salte abruptamente
    for (int i = 0; i < kFftSize; ++i)
    {
        const float mag = fftData[(size_t) i];
        magsSmoothed[(size_t) i] = magsSmoothed[(size_t) i] * 0.5f + mag * 0.5f;
    }

    repaint();
}

void SpectrumComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    const float pad = 6.0f;
    const float plotX0 = bounds.getX() + pad;
    const float plotX1 = bounds.getRight() - 20.0f;
    const float plotY0 = bounds.getY() + pad;
    const float plotY1 = bounds.getBottom() - pad;
    const float plotW  = plotX1 - plotX0;
    const float plotH  = plotY1 - plotY0;

    const double sr = bufferRef.sampleRate.load(std::memory_order_relaxed);
    const double nyquist = sr * 0.5;

    // Rango visual de frecuencia (log) y dB
    const float fMin = 20.0f;
    const float fMax = (float) juce::jmin(20000.0, nyquist);
    const float dBMin = -80.0f;
    const float dBMax = 0.0f;

    // Grilla logaritmica con etiquetas en 100, 1k, 10k
    g.setColour(juce::Colour(0xff242438));
    for (float freq : { 100.0f, 1000.0f, 10000.0f })
    {
        if (freq < fMin || freq > fMax) continue;
        const float fx = plotX0 + (std::log10(freq / fMin) / std::log10(fMax / fMin)) * plotW;
        g.drawVerticalLine((int) fx, plotY0, plotY1);
    }
    // Grilla horizontal (dB) cada 20 dB
    for (int dB = -20; dB >= -60; dB -= 20)
    {
        const float fy = juce::jmap((float) dB, dBMin, dBMax, plotY1, plotY0);
        g.drawHorizontalLine((int) fy, plotX0, plotX1);
    }

    // Construir path del espectro
    juce::Path spectrum;
    bool first = true;

    // Normalizacion de la FFT: dividimos por (kFftSize / 2) para que un seno
    // amplitud 1 quede aprox a 0 dB.
    const float fftNorm = 2.0f / (float) kFftSize;

    // Para cada x del plot, encontrar el bin correspondiente (con interp logaritmica)
    const int numBins = kFftSize / 2;
    for (int px = 0; px <= (int) plotW; ++px)
    {
        const float t = (float) px / plotW;
        const float freq = fMin * std::pow(fMax / fMin, t); // log
        const float bin = (float) freq / (float) nyquist * (float) numBins;
        const int b0 = juce::jlimit(0, numBins - 2, (int) bin);
        const float frac = bin - (float) b0;
        const float mag = magsSmoothed[(size_t) b0] * (1.0f - frac)
                        + magsSmoothed[(size_t)(b0 + 1)] * frac;
        const float magNorm = mag * fftNorm;
        const float dB = juce::Decibels::gainToDecibels(magNorm + 1.0e-6f);
        const float dBClamped = juce::jlimit(dBMin, dBMax, dB);
        const float x = plotX0 + (float) px;
        const float y = juce::jmap(dBClamped, dBMin, dBMax, plotY1, plotY0);
        if (first) { spectrum.startNewSubPath(x, y); first = false; }
        else       { spectrum.lineTo(x, y); }
    }

    // Cerrar el path para rellenar bajo la curva
    juce::Path fillPath = spectrum;
    fillPath.lineTo(plotX1, plotY1);
    fillPath.lineTo(plotX0, plotY1);
    fillPath.closeSubPath();

    g.setColour(juce::Colour(0xff7a9dff).withAlpha(0.20f));
    g.fillPath(fillPath);

    g.setColour(juce::Colour(0xff7a9dff));
    g.strokePath(spectrum, juce::PathStrokeType(1.4f, juce::PathStrokeType::curved));

    // Etiquetas de frecuencia
    g.setColour(juce::Colours::darkgrey);
    g.setFont(juce::Font(8.5f));
    auto drawFreqLabel = [&](float freq, const char* label)
    {
        if (freq < fMin || freq > fMax) return;
        const float fx = plotX0 + (std::log10(freq / fMin) / std::log10(fMax / fMin)) * plotW;
        g.drawText(label, juce::Rectangle<int>((int) fx - 18, (int) plotY1 - 12, 36, 12),
                   juce::Justification::centred);
    };
    drawFreqLabel(100.0f,   "100");
    drawFreqLabel(1000.0f,  "1k");
    drawFreqLabel(10000.0f, "10k");

    // Curva del filtro superpuesta (si se ha pasado el APVTS)
    if (apvtsRef != nullptr)
    {
        const int   fType   = (int) apvtsRef->getRawParameterValue("FILTER_TYPE")->load();
        const float fCutoff =       apvtsRef->getRawParameterValue("FILTER_CUTOFF")->load();
        const float fRes    =       apvtsRef->getRawParameterValue("FILTER_RES")->load();

        // La curva del filtro tambien usa el eje X log de frecuencia, pero su
        // eje Y se mapea a un rango distinto (la curva oscila entre -36 y +18 dB
        // aprox). La normalizamos al area visible.
        juce::Path filterPath;
        bool ffirst = true;
        for (int px = 0; px <= (int) plotW; ++px)
        {
            const float t = (float) px / plotW;
            const float freq = fMin * std::pow(fMax / fMin, t);
            const double magLin = FilterData::getMagnitudeAtFrequency(fType, fCutoff, fRes, freq, sr);
            const float magDb = (float) juce::Decibels::gainToDecibels(magLin + 1.0e-9);
            // Mapeamos -60 a +18 dB del filtro al rango visible
            const float magClamped = juce::jlimit(-60.0f, 18.0f, magDb);
            const float y = juce::jmap(magClamped, -60.0f, 18.0f, plotY1, plotY0);
            const float x = plotX0 + (float) px;
            if (ffirst) { filterPath.startNewSubPath(x, y); ffirst = false; }
            else        { filterPath.lineTo(x, y); }
        }
        g.setColour(juce::Colour(0xff8effc1).withAlpha(0.85f));
        g.strokePath(filterPath, juce::PathStrokeType(1.6f, juce::PathStrokeType::curved));

        // Marcador del cutoff
        const float cutoffX = plotX0 + (std::log10(juce::jlimit(fMin, fMax, fCutoff) / fMin)
                                        / std::log10(fMax / fMin)) * plotW;
        g.setColour(juce::Colour(0xff8effc1).withAlpha(0.45f));
        g.drawVerticalLine((int) cutoffX, plotY0, plotY1);
    }

    // Etiqueta SPECTRUM en esquina
    g.setColour(juce::Colour(0xff7a9dff));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("SPECTRUM", bounds.toNearestInt().reduced(8, 4),
               juce::Justification::topLeft);

    // Leyenda del overlay
    if (apvtsRef != nullptr)
    {
        g.setColour(juce::Colour(0xff8effc1).withAlpha(0.7f));
        g.setFont(juce::Font(9.0f));
        g.drawText("filter response",
                   bounds.toNearestInt().reduced(8, 4),
                   juce::Justification::topRight);
    }
}
