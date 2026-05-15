/*
  ==============================================================================

    MixerVisualizer.cpp

  ==============================================================================
*/

#include "MixerVisualizer.h"

MixerVisualizer::MixerVisualizer(juce::AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    startTimerHz(25);
}

MixerVisualizer::~MixerVisualizer()
{
    stopTimer();
}

void MixerVisualizer::timerCallback()
{
    repaint();
}

float MixerVisualizer::renderWave(int waveType, float phase, float pw)
{
    // phase debe estar en [0,1). La ajustamos por si acaso.
    phase -= std::floor(phase);

    switch (waveType)
    {
        case 0: return std::sin(phase * juce::MathConstants<float>::twoPi);      // Sine
        case 1: return (phase < 0.5f) ? (-1.0f + 4.0f * phase)                   // Triangle
                                      : ( 3.0f - 4.0f * phase);
        case 2: return 2.0f * phase - 1.0f;                                      // Saw
        case 3: return (phase < 0.5f) ? 1.0f : -1.0f;                            // Square
        case 4: return (phase < pw)   ? 1.0f : -1.0f;                            // Pulse
        case 5: // Noise (forma decorativa pseudo-aleatoria)
            return std::sin(phase * 47.0f) * std::cos(phase * 73.0f);
    }
    return 0.0f;
}

void MixerVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    // Fondo
    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    // Eje cero
    const float midY = bounds.getCentreY();
    g.setColour(juce::Colour(0xff242438));
    g.drawHorizontalLine((int)midY, bounds.getX() + 4, bounds.getRight() - 4);

    // Lee todos los parametros relevantes
    auto P = [&](const char* id) { return apvtsRef.getRawParameterValue(id)->load(); };
    const int   w1   = (int)P("OSC1WAVETYPE");
    const int   w2   = (int)P("OSC2WAVETYPE");
    const float pw1  = P("OSC1PW");
    const float pw2  = P("OSC2PW");

    // Detune relativo de OSC2 vs OSC1 (en semitonos), para mostrar batidos visualmente
    const float octDiff  = (float)((int)P("OSC2OCTAVE") - (int)P("OSC1OCTAVE"));
    const float semiDiff = (float)((int)P("OSC2SEMI")   - (int)P("OSC1SEMI"));
    const float fineDiff = (P("OSC2FINE") - P("OSC1FINE")) / 100.0f;
    const float semitonesDiff = octDiff * 12.0f + semiDiff + fineDiff;
    const float ratio2 = std::pow(2.0f, semitonesDiff / 12.0f); // factor de frecuencia OSC2/OSC1

    const float mix    = P("OSC_MIX");
    const int   mode   = (int)P("OSC_MODE");
    const float fmAmt  = P("FM_AMOUNT");

    const int   subW   = (int)P("SUB_WAVE");
    const float subLvl = P("SUB_LEVEL");

    // Dibujamos N muestras representando ~3 ciclos del OSC1 (para que se vea el batido)
    const float cycles = 3.0f;
    const int   N = 360;

    const float padding = 6.0f;
    const float x0 = bounds.getX() + padding;
    const float x1 = bounds.getRight() - padding;
    const float plotW = x1 - x0;
    const float amp = (bounds.getHeight() - padding * 2.0f) * 0.45f;

    // Estado para hard-sync
    float lastPhase1 = 0.0f;
    float syncPhase2 = 0.0f;

    juce::Path tracePath;
    bool first = true;

    for (int i = 0; i < N; ++i)
    {
        const float t = (float)i / (float)(N - 1) * cycles; // posicion en ciclos de OSC1

        float phase1 = t - std::floor(t);
        float phase2 = (t * ratio2) - std::floor(t * ratio2);

        // Hard-sync: cuando OSC1 wraps, OSC2 se resetea
        if (mode == 2 /* Sync */)
        {
            if (phase1 < lastPhase1) syncPhase2 = 0.0f;
            syncPhase2 += (ratio2 / (float)(N - 1)) * cycles;
            syncPhase2 -= std::floor(syncPhase2);
            phase2 = syncPhase2;
        }
        lastPhase1 = phase1;

        // FM: OSC2 modula la fase de OSC1
        float s1Phase = phase1;
        if (mode == 3 /* FM */)
        {
            const float modulator = renderWave(w2, phase2, pw2);
            s1Phase = phase1 + modulator * fmAmt * 0.5f;
        }
        const float s1 = renderWave(w1, s1Phase, pw1);
        const float s2 = renderWave(w2, phase2, pw2);

        float combined = 0.0f;
        switch (mode)
        {
            case 0: combined = (1.0f - mix) * s1 + mix * s2;  break; // Mix
            case 1: combined = s1 * s2;                       break; // Ring
            case 2: combined = (1.0f - mix) * s1 + mix * s2;  break; // Sync
            case 3: combined = s1;                            break; // FM (solo portador)
        }

        // Sumar sub-oscilador (1 octava abajo => frecuencia 0.5x)
        const float phaseSub = (t * 0.5f) - std::floor(t * 0.5f);
        combined += renderWave(subW, phaseSub, 0.5f) * subLvl;

        const float v = juce::jlimit(-1.5f, 1.5f, combined);
        const float x = juce::jmap((float)i / (float)(N - 1), 0.0f, 1.0f, x0, x1);
        const float y = midY - v * amp * 0.7f; // 0.7 para dar un poco de margen visual
        if (first) { tracePath.startNewSubPath(x, y); first = false; }
        else        { tracePath.lineTo(x, y); }
    }

    g.setColour(juce::Colour(0xff7a9dff));
    g.strokePath(tracePath, juce::PathStrokeType(1.6f, juce::PathStrokeType::curved));

    // Pequena etiqueta del modo en la esquina
    const char* modeNames[] = { "MIX", "RING", "SYNC", "FM" };
    g.setColour(juce::Colour(0xff7a9dff).withAlpha(0.85f));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText(modeNames[juce::jlimit(0, 3, mode)],
               bounds.toNearestInt().reduced(6, 4),
               juce::Justification::topRight);
}
