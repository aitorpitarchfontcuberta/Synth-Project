/*
  ==============================================================================

    FxComponent.cpp

    Cada bloque FX (Chorus/Delay/Reverb) tiene:
      - Boton ON/OFF
      - Sus knobs especificos
      - Un mini visualizador didactico que dibuja su comportamiento.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FxComponent.h"

//==============================================================================
// FxBlock: encapsula un efecto generico con titulo, on/off, knobs y un
// visualizador opcional que se redibuja a 20fps.

class FxComponent::FxBlock : public juce::Component, private juce::Timer
{
public:
    enum class Kind { Chorus, Delay, Reverb };

    FxBlock(Kind k, juce::AudioProcessorValueTreeState& apvtsRef)
        : kind(k), apvts(apvtsRef)
    {
        const char* titleStr  = "";
        const char* enableID  = "";
        switch (kind)
        {
            case Kind::Chorus: titleStr = "CHORUS"; enableID = "CHORUS_ON"; break;
            case Kind::Delay:  titleStr = "DELAY";  enableID = "DELAY_ON";  break;
            case Kind::Reverb: titleStr = "REVERB"; enableID = "REVERB_ON"; break;
        }

        titleLabel.setText(titleStr, juce::dontSendNotification);
        titleLabel.setFont(juce::Font(14.0f, juce::Font::bold));
        titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff7a9dff));
        addAndMakeVisible(titleLabel);

        onBtn.setButtonText("ON");
        onBtn.setClickingTogglesState(true);
        onBtn.setColour(juce::TextButton::buttonColourId,   juce::Colour(0xff2a2a3a));
        onBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff7a9dff));
        onBtn.setColour(juce::TextButton::textColourOffId,  juce::Colours::lightgrey);
        onBtn.setColour(juce::TextButton::textColourOnId,   juce::Colours::white);
        addAndMakeVisible(onBtn);
        onAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts, enableID, onBtn);

        // Construir knobs especificos del efecto
        switch (kind)
        {
            case Kind::Chorus:
                makeKnob(k1, k1L, "Rate",  "CHORUS_RATE",  " Hz");
                makeKnob(k2, k2L, "Depth", "CHORUS_DEPTH", "");
                makeKnob(k3, k3L, "Fb",    "CHORUS_FB",    "");
                makeKnob(k4, k4L, "Mix",   "CHORUS_MIX",   "");
                break;
            case Kind::Delay:
                makeKnob(k1, k1L, "Time", "DELAY_TIME", " ms");
                makeKnob(k2, k2L, "Fb",   "DELAY_FB",   "");
                makeKnob(k3, k3L, "Mix",  "DELAY_MIX",  "");
                break;
            case Kind::Reverb:
                makeKnob(k1, k1L, "Size",  "REVERB_SIZE",  "");
                makeKnob(k2, k2L, "Damp",  "REVERB_DAMP",  "");
                makeKnob(k3, k3L, "Width", "REVERB_WIDTH", "");
                makeKnob(k4, k4L, "Mix",   "REVERB_MIX",   "");
                break;
        }

        startTimerHz(20);
    }

    ~FxBlock() override { stopTimer(); }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        juce::ColourGradient grad(juce::Colour(0xff2a2a3a), bounds.getTopLeft(),
                                  juce::Colour(0xff1a1a25), bounds.getBottomRight(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(bounds, 8.0f);
        g.setColour(juce::Colour(0xff4a4a6a));
        g.drawRoundedRectangle(bounds, 8.0f, 1.5f);

        // Visualizador
        drawVisualizer(g, visualizerBounds.toFloat().reduced(2.0f));
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced(8);

        // Fila titulo + on/off
        auto top = b.removeFromTop(22);
        titleLabel.setBounds(top.removeFromLeft(80));
        onBtn.setBounds(top.removeFromLeft(40));

        b.removeFromTop(4);

        // Visualizador a la izquierda (60% del ancho), knobs a la derecha
        const int visualW = (int) (b.getWidth() * 0.60f);
        visualizerBounds = b.removeFromLeft(visualW);
        b.removeFromLeft(8);

        const int labelH = 14;
        const int n      = numKnobs();
        const int w      = b.getWidth() / n;
        auto layoutKnob = [&](juce::Slider& s, juce::Label& l, int i)
        {
            auto a = juce::Rectangle<int>(b.getX() + i * w, b.getY(), w, b.getHeight());
            l.setBounds(a.removeFromBottom(labelH));
            s.setBounds(a.reduced(2));
        };
        if (n >= 1) layoutKnob(k1, k1L, 0);
        if (n >= 2) layoutKnob(k2, k2L, 1);
        if (n >= 3) layoutKnob(k3, k3L, 2);
        if (n >= 4) layoutKnob(k4, k4L, 3);
    }

private:
    void timerCallback() override { repaint(); }

    int numKnobs() const
    {
        return kind == Kind::Delay ? 3 : 4;
    }

    void makeKnob(juce::Slider& s, juce::Label& l, const juce::String& text,
                  const juce::String& id, const juce::String& suffix)
    {
        s.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 14);
        s.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xff7a9dff));
        s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a50));
        s.setColour(juce::Slider::thumbColourId,               juce::Colours::white);
        s.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
        s.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
        if (suffix.isNotEmpty()) s.setTextValueSuffix(suffix);
        addAndMakeVisible(s);

        l.setText(text, juce::dontSendNotification);
        l.setFont(juce::Font(11.0f, juce::Font::bold));
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible(l);

        auto attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts, id, s);
        attachments.push_back(std::move(attach));
    }

    void drawVisualizer(juce::Graphics& g, juce::Rectangle<float> b)
    {
        g.setColour(juce::Colour(0xff10101a));
        g.fillRoundedRectangle(b, 4.0f);
        g.setColour(juce::Colour(0xff3a3a50));
        g.drawRoundedRectangle(b, 4.0f, 1.0f);

        const bool isOn = onBtn.getToggleState();
        const float alpha = isOn ? 1.0f : 0.35f;

        switch (kind)
        {
            case Kind::Chorus: drawChorusViz(g, b, alpha); break;
            case Kind::Delay:  drawDelayViz (g, b, alpha); break;
            case Kind::Reverb: drawReverbViz(g, b, alpha); break;
        }
    }

    // --- Chorus: dos osciladores desfasados que evidencian el efecto de modulacion ---
    void drawChorusViz(juce::Graphics& g, juce::Rectangle<float> b, float alpha)
    {
        const float rate  = apvts.getRawParameterValue("CHORUS_RATE")->load();
        const float depth = apvts.getRawParameterValue("CHORUS_DEPTH")->load();
        const float mix   = apvts.getRawParameterValue("CHORUS_MIX")->load();

        const float midY = b.getCentreY();
        g.setColour(juce::Colour(0xff242438));
        g.drawHorizontalLine((int)midY, b.getX() + 4, b.getRight() - 4);

        const float pad = 6.0f;
        const float x0 = b.getX() + pad;
        const float x1 = b.getRight() - pad;
        const float plotW = x1 - x0;
        const float amp = b.getHeight() * 0.35f;
        const int N = 200;

        // Onda original (gris)
        juce::Path orig;
        for (int i = 0; i <= N; ++i)
        {
            const float t = (float)i / (float)N;
            const float v = std::sin(t * juce::MathConstants<float>::twoPi * 3.5f);
            const float x = juce::jmap(t, 0.0f, 1.0f, x0, x1);
            const float y = midY - v * amp;
            if (i == 0) orig.startNewSubPath(x, y);
            else        orig.lineTo(x, y);
        }
        g.setColour(juce::Colours::darkgrey.withAlpha(alpha * 0.6f));
        g.strokePath(orig, juce::PathStrokeType(1.0f));

        // Onda modulada (chorus) - misma onda con desfase ondulante segun rate y depth
        juce::Path chorused;
        for (int i = 0; i <= N; ++i)
        {
            const float t = (float)i / (float)N;
            const float modPhase = std::sin(t * juce::MathConstants<float>::twoPi * rate * 0.4f);
            const float offset = modPhase * depth * 0.15f;
            const float v = std::sin((t + offset) * juce::MathConstants<float>::twoPi * 3.5f);
            const float vMixed = v * mix + std::sin(t * juce::MathConstants<float>::twoPi * 3.5f) * (1.0f - mix);
            const float x = juce::jmap(t, 0.0f, 1.0f, x0, x1);
            const float y = midY - vMixed * amp;
            if (i == 0) chorused.startNewSubPath(x, y);
            else        chorused.lineTo(x, y);
        }
        g.setColour(juce::Colour(0xff7a9dff).withAlpha(alpha));
        g.strokePath(chorused, juce::PathStrokeType(1.6f, juce::PathStrokeType::curved));
    }

    // --- Delay: train de echos espaciados temporalmente, decreciendo segun feedback ---
    void drawDelayViz(juce::Graphics& g, juce::Rectangle<float> b, float alpha)
    {
        const float time = apvts.getRawParameterValue("DELAY_TIME")->load();
        const float fb   = apvts.getRawParameterValue("DELAY_FB")->load();
        const float mix  = apvts.getRawParameterValue("DELAY_MIX")->load();

        const float pad = 8.0f;
        const float x0 = b.getX() + pad;
        const float x1 = b.getRight() - pad;
        const float plotW = x1 - x0;
        const float baseY = b.getBottom() - pad;
        const float maxH  = b.getHeight() * 0.7f;

        // Eje de tiempo
        g.setColour(juce::Colour(0xff242438));
        g.drawHorizontalLine((int)baseY, x0, x1);

        // Asumimos una ventana visible de 1500ms para que delays largos quepan
        const float windowMs = 1500.0f;
        const int   maxEchoes = 12;

        // Echo inicial (dry, en t=0)
        float currentX = x0;
        float currentAmp = 1.0f;
        for (int i = 0; i < maxEchoes; ++i)
        {
            float echoTime = i * time;
            if (echoTime > windowMs) break;
            float echoX = juce::jmap(echoTime / windowMs, 0.0f, 1.0f, x0, x1);
            float h = currentAmp * maxH;

            // Echo 0 es la senial seca: la pintamos blanca; los demas en azul.
            if (i == 0)
            {
                g.setColour(juce::Colours::white.withAlpha(alpha));
                g.drawLine(echoX, baseY, echoX, baseY - h, 1.5f);
            }
            else
            {
                const float echoIntensity = currentAmp * mix;
                g.setColour(juce::Colour(0xff7a9dff).withAlpha(alpha * juce::jmin(1.0f, echoIntensity * 1.4f)));
                g.drawLine(echoX, baseY, echoX, baseY - h, 1.5f);
            }
            currentAmp *= fb;
        }

        // Etiqueta de tiempo
        g.setColour(juce::Colours::darkgrey.withAlpha(alpha));
        g.setFont(juce::Font(8.5f));
        g.drawText("0", juce::Rectangle<int>((int)x0 - 8, (int)baseY + 1, 16, 12),
                   juce::Justification::centredLeft);
        g.drawText("1.5s", juce::Rectangle<int>((int)x1 - 18, (int)baseY + 1, 20, 12),
                   juce::Justification::centredRight);
    }

    // --- Reverb: cola exponencial decreciente, longitud y densidad segun size/damping ---
    void drawReverbViz(juce::Graphics& g, juce::Rectangle<float> b, float alpha)
    {
        const float size   = apvts.getRawParameterValue("REVERB_SIZE")->load();
        const float damp   = apvts.getRawParameterValue("REVERB_DAMP")->load();
        const float mix    = apvts.getRawParameterValue("REVERB_MIX")->load();

        const float pad = 8.0f;
        const float x0 = b.getX() + pad;
        const float x1 = b.getRight() - pad;
        const float plotW = x1 - x0;
        const float baseY = b.getBottom() - pad;
        const float maxH  = b.getHeight() * 0.7f;

        // Eje
        g.setColour(juce::Colour(0xff242438));
        g.drawHorizontalLine((int)baseY, x0, x1);

        // Estimacion didactica: tiempo de decaimiento ~ size * 3 segundos.
        // Densidad de reflexiones ~ 30..120 segun size.
        const float decayTime = 0.3f + size * 3.0f;
        const int numReflections = 30 + (int)(size * 90.0f);

        juce::Random rng(123);
        for (int i = 0; i < numReflections; ++i)
        {
            const float t = (float)i / (float)numReflections;
            // Tiempo no-uniforme: primeras reflexiones discretas, luego densas
            const float jitter = rng.nextFloat() * 0.4f - 0.2f;
            const float t2 = juce::jlimit(0.0f, 1.0f, t + jitter * 0.05f);
            const float x = juce::jmap(t2, 0.0f, 1.0f, x0, x1);

            // Amplitud: decae exponencialmente; damp reduce mas rapido las altas
            // pero aqui solo modelamos la curva general.
            const float damping = 1.0f - damp * 0.6f;
            const float amp = std::pow(1.0f - t2, 1.0f + (1.0f - damping) * 2.0f);
            const float h = amp * maxH * (0.5f + 0.5f * rng.nextFloat()) * mix;

            g.setColour(juce::Colour(0xff7a9dff).withAlpha(alpha * juce::jmin(1.0f, amp + 0.1f)));
            g.drawLine(x, baseY, x, baseY - h, 1.0f);
        }

        // Etiqueta del tiempo total
        g.setColour(juce::Colours::darkgrey.withAlpha(alpha));
        g.setFont(juce::Font(8.5f));
        g.drawText("0", juce::Rectangle<int>((int)x0 - 8, (int)baseY + 1, 16, 12),
                   juce::Justification::centredLeft);
        g.drawText(juce::String(decayTime, 1) + "s",
                   juce::Rectangle<int>((int)x1 - 28, (int)baseY + 1, 28, 12),
                   juce::Justification::centredRight);
    }

    Kind kind;
    juce::AudioProcessorValueTreeState& apvts;

    juce::Label      titleLabel;
    juce::TextButton onBtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> onAttachment;

    juce::Slider k1, k2, k3, k4;
    juce::Label  k1L, k2L, k3L, k4L;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

    juce::Rectangle<int> visualizerBounds;
};

//==============================================================================
FxComponent::FxComponent(juce::AudioProcessorValueTreeState& apvts)
{
    chorusBlock = std::make_unique<FxBlock>(FxBlock::Kind::Chorus, apvts);
    delayBlock  = std::make_unique<FxBlock>(FxBlock::Kind::Delay,  apvts);
    reverbBlock = std::make_unique<FxBlock>(FxBlock::Kind::Reverb, apvts);
    addAndMakeVisible(*chorusBlock);
    addAndMakeVisible(*delayBlock);
    addAndMakeVisible(*reverbBlock);
}

FxComponent::~FxComponent() {}

void FxComponent::paint(juce::Graphics&) {}

void FxComponent::resized()
{
    auto b = getLocalBounds().reduced(6);
    const int gap = 6;
    const int rowH = (b.getHeight() - gap * 2) / 3;

    chorusBlock->setBounds(b.removeFromTop(rowH));
    b.removeFromTop(gap);
    delayBlock ->setBounds(b.removeFromTop(rowH));
    b.removeFromTop(gap);
    reverbBlock->setBounds(b);
}
