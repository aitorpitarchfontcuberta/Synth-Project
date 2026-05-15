/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/AdsrComponent.h"
#include "UI/OscComponent.h"
#include "UI/LfoComponent.h"
#include "UI/ModEnvComponent.h"
#include "UI/MixerComponent.h"
#include "UI/MasterComponent.h"
#include "UI/FilterComponent.h"
#include "UI/FxComponent.h"
#include "UI/OscilloscopeComponent.h"
#include "UI/SpectrumComponent.h"
#include "UI/PresetBarComponent.h"

class SynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SynthAudioProcessorEditor(SynthAudioProcessor&);
    ~SynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SynthAudioProcessor& audioProcessor;

    // === Contenido de cada pestania ===
    class OscillatorsPage : public juce::Component
    {
    public:
        OscillatorsPage(juce::AudioProcessorValueTreeState& apvts)
            : osc1(apvts, "OSC 1", "OSC1"),
              osc2(apvts, "OSC 2", "OSC2"),
              mixer(apvts)
        {
            addAndMakeVisible(osc1);
            addAndMakeVisible(osc2);
            addAndMakeVisible(mixer);
        }
        void resized() override
        {
            auto b = getLocalBounds().reduced(6);
            const int gap = 6;
            const int colW = (b.getWidth() - gap * 2) / 3;
            osc1.setBounds (b.removeFromLeft(colW)); b.removeFromLeft(gap);
            osc2.setBounds (b.removeFromLeft(colW)); b.removeFromLeft(gap);
            mixer.setBounds(b);
        }
    private:
        OscComponent   osc1, osc2;
        MixerComponent mixer;
    };

    class ModulationPage : public juce::Component
    {
    public:
        ModulationPage(juce::AudioProcessorValueTreeState& apvts)
            : lfo1(apvts, "LFO 1", "LFO1"),
              lfo2(apvts, "LFO 2", "LFO2"),
              modEnv(apvts)
        {
            addAndMakeVisible(lfo1);
            addAndMakeVisible(lfo2);
            addAndMakeVisible(modEnv);
        }
        void resized() override
        {
            auto b = getLocalBounds().reduced(6);
            const int gap = 6;
            // Mod Env algo mas ancho (~38%) que los LFOs (~31% c/u)
            const int totalW = b.getWidth() - gap * 2;
            const int lfoW = (int)(totalW * 0.31f);
            const int modW = totalW - lfoW * 2;
            lfo1.setBounds  (b.removeFromLeft(lfoW)); b.removeFromLeft(gap);
            lfo2.setBounds  (b.removeFromLeft(lfoW)); b.removeFromLeft(gap);
            modEnv.setBounds(b.removeFromLeft(modW));
        }
    private:
        LfoComponent    lfo1, lfo2;
        ModEnvComponent modEnv;
    };

    class EnvelopePage : public juce::Component
    {
    public:
        EnvelopePage(juce::AudioProcessorValueTreeState& apvts) : ampAdsr(apvts)
        {
            addAndMakeVisible(ampAdsr);
        }
        void resized() override { ampAdsr.setBounds(getLocalBounds().reduced(6)); }
    private:
        AdsrComponent ampAdsr;
    };

    class MasterPage : public juce::Component
    {
    public:
        MasterPage(juce::AudioProcessorValueTreeState& apvts) : master(apvts)
        {
            addAndMakeVisible(master);
        }
        void resized() override { master.setBounds(getLocalBounds().reduced(6)); }
    private:
        MasterComponent master;
    };

    class FilterPage : public juce::Component
    {
    public:
        FilterPage(juce::AudioProcessorValueTreeState& apvts, const ScopeBuffer& scopeBuffer)
            : filter(apvts, scopeBuffer)
        {
            addAndMakeVisible(filter);
        }
        void resized() override { filter.setBounds(getLocalBounds().reduced(6)); }
    private:
        FilterComponent filter;
    };

    class FxPage : public juce::Component
    {
    public:
        FxPage(juce::AudioProcessorValueTreeState& apvts) : fx(apvts)
        {
            addAndMakeVisible(fx);
        }
        void resized() override { fx.setBounds(getLocalBounds().reduced(6)); }
    private:
        FxComponent fx;
    };

    OscillatorsPage page1 { audioProcessor.apvts };
    ModulationPage  page2 { audioProcessor.apvts };
    EnvelopePage    page3 { audioProcessor.apvts };
    FilterPage      page5 { audioProcessor.apvts, audioProcessor.scopeBuffer };
    FxPage          page6 { audioProcessor.apvts };
    MasterPage      page4 { audioProcessor.apvts };

    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };

    // Barra de presets en la parte superior
    PresetBarComponent presetBar { audioProcessor.presetManager };

    // Panel inferior con dos vistas: Waveform y Spectrum
    OscilloscopeComponent scope    { audioProcessor.scopeBuffer };
    SpectrumComponent     spectrum { audioProcessor.scopeBuffer, &audioProcessor.apvts };
    juce::TabbedComponent outputTabs { juce::TabbedButtonBar::TabsAtTop };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessorEditor)
};
