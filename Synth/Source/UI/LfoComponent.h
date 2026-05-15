/*
  ==============================================================================

    LfoComponent.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LfoVisualizer.h"

class LfoComponent : public juce::Component
{
public:
    LfoComponent(juce::AudioProcessorValueTreeState& apvts,
                 const juce::String& titleText,
                 const juce::String& paramPrefix);
    ~LfoComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParams(juce::Slider& s);
    void setLabelParams(juce::Label& l, const juce::String& t);

    juce::String title;
    juce::Label titleLabel;
    juce::Label waveLabel, destLabel;
    juce::ComboBox waveBox, destBox;
    std::unique_ptr<LfoVisualizer> waveVis;

    juce::Slider rateSlider, depthSlider;
    juce::Label  rateLabel,  depthLabel;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<CA> waveAttach, destAttach;
    std::unique_ptr<SA> rateAttach, depthAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfoComponent)
};
