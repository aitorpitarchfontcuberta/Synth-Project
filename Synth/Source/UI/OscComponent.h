/*
  ==============================================================================

    OscComponent.h
    Created: 12 May 2026 9:34:43pm
    Author:  Aitor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WaveformVisualizer.h"

class ParamInfoManager;

class OscComponent : public juce::Component
{
public:
    // titleText: titulo visible (p.ej. "OSC 1")
    // paramPrefix: prefijo de los IDs APVTS (p.ej. "OSC1" -> "OSC1WAVETYPE", "OSC1OCTAVE"...)
    OscComponent(juce::AudioProcessorValueTreeState& apvts,
                 const juce::String& titleText,
                 const juce::String& paramPrefix,
                 ParamInfoManager* infoManager = nullptr);
    ~OscComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParams(juce::Slider& slider);
    void setLabelParams(juce::Label& label, const juce::String& text);

    juce::String title;
    juce::Label  titleLabel;
    juce::Label  waveLabel;
    juce::ComboBox oscWaveSelector;
    std::unique_ptr<WaveformVisualizer> waveVis;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oscWaveSelectorAttachment;

    juce::Slider octaveSlider, semiSlider, fineSlider, pwSlider;
    juce::Label  octaveLabel,  semiLabel,  fineLabel,  pwLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> octaveAttachment, semiAttachment, fineAttachment, pwAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscComponent)
};
