/*
  ==============================================================================

    MixerComponent.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MixBarVisualizer.h"
#include "MixerVisualizer.h"

class MixerComponent : public juce::Component
{
public:
    MixerComponent(juce::AudioProcessorValueTreeState& apvts);
    ~MixerComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParams(juce::Slider& s, juce::Colour colour);
    void setLabelParams(juce::Label& l, const juce::String& t);

    juce::Label titleLabel;
    std::unique_ptr<MixerVisualizer> mixerVis;
    std::unique_ptr<MixBarVisualizer> mixBar;

    // Mix + Mode + FM
    juce::Slider mixSlider, fmSlider, subLevelSlider;
    juce::Label  mixLabel,  fmLabel,  subLevelLabel;

    juce::Label modeLabel, subWaveLabel;
    juce::ComboBox modeBox, subWaveBox;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SA> mixA, fmA, subLevelA;
    std::unique_ptr<CA> modeA, subWaveA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerComponent)
};
