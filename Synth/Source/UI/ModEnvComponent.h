/*
  ==============================================================================

    ModEnvComponent.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AdsrVisualizer.h"

class ParamInfoManager;

class ModEnvComponent : public juce::Component
{
public:
    ModEnvComponent(juce::AudioProcessorValueTreeState& apvts,
                    ParamInfoManager* infoManager = nullptr);
    ~ModEnvComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParams(juce::Slider& s);
    void setLabelParams(juce::Label& l, const juce::String& t);

    juce::Label titleLabel;
    std::unique_ptr<AdsrVisualizer> visualizer;

    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, depthSlider;
    juce::Label  attackLabel,  decayLabel,  sustainLabel,  releaseLabel,  depthLabel;

    juce::Label destLabel;
    juce::ComboBox destBox;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SA> aA, dA, sA, rA, depthA;
    std::unique_ptr<CA> destA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModEnvComponent)
};
