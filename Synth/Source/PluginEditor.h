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
#include "UI/ParamInfoManager.h"

class SynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SynthAudioProcessorEditor(SynthAudioProcessor&);
    ~SynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SynthAudioProcessor& audioProcessor;

    // Manager global del sistema de info de parametros (click derecho).
    ParamInfoManager infoManager;

    // === Contenido de cada pestania ===
    class OscillatorsPage : public juce::Component
    {
    public:
        OscillatorsPage(juce::AudioProcessorValueTreeState& apvts, ParamInfoManager* info)
            : osc1(apvts, "OSC 1", "OSC1", info),
              osc2(apvts, "OSC 2", "OSC2", info),
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
        ModulationPage(juce::AudioProcessorValueTreeState& apvts, ParamInfoManager* info)
            : lfo1(apvts, "LFO 1", "LFO1"),
              lfo2(apvts, "LFO 2", "LFO2"),
              modEnv(apvts, info)
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

    // Pestania combinada: Master a la izquierda, Envelope a la derecha
    class MasterEnvelopePage : public juce::Component
    {
    public:
        MasterEnvelopePage(juce::AudioProcessorValueTreeState& apvts, ParamInfoManager* info)
            : master(apvts), ampAdsr(apvts, info)
        {
            addAndMakeVisible(master);
            addAndMakeVisible(ampAdsr);
        }
        void resized() override
        {
            auto b = getLocalBounds().reduced(6);
            const int gap = 8;
            // Master ~50% izquierda, Envelope el resto
            const int masterW = (b.getWidth() - gap) / 2;
            master.setBounds (b.removeFromLeft(masterW));
            b.removeFromLeft(gap);
            ampAdsr.setBounds(b);
        }
    private:
        MasterComponent master;
        AdsrComponent   ampAdsr;
    };

    class FilterPage : public juce::Component
    {
    public:
        FilterPage(juce::AudioProcessorValueTreeState& apvts, const ScopeBuffer& scopeBuffer,
                   ParamInfoManager* info)
            : filter(apvts, scopeBuffer, info)
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

    OscillatorsPage    page1 { audioProcessor.apvts, &infoManager };
    ModulationPage     page2 { audioProcessor.apvts, &infoManager };
    FilterPage         page5 { audioProcessor.apvts, audioProcessor.scopeBuffer, &infoManager };
    FxPage             page6 { audioProcessor.apvts };
    MasterEnvelopePage page4 { audioProcessor.apvts, &infoManager };

    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };

    // Barra de presets en la parte superior
    PresetBarComponent presetBar { audioProcessor.presetManager };

    // Panel inferior con dos vistas: Waveform y Spectrum
    OscilloscopeComponent scope    { audioProcessor.scopeBuffer };
    SpectrumComponent     spectrum { audioProcessor.scopeBuffer, &audioProcessor.apvts };
    juce::TabbedComponent outputTabs { juce::TabbedButtonBar::TabsAtTop };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessorEditor)
};
