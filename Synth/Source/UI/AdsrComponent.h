/*
  ==============================================================================

    AdsrComponent.h
    Created: 30 Apr 2026 11:36:06am
    Author:  Aitor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AdsrVisualizer.h"

class ParamInfoManager;

//==============================================================================
/*
*/
class AdsrComponent  : public juce::Component
{
public:
    AdsrComponent(juce::AudioProcessorValueTreeState& apvts,
                  ParamInfoManager* infoManager = nullptr);
    ~AdsrComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParams(juce::Slider& slider);
    void setLabelParams(juce::Label& label, const juce::String& text);

    juce::Label titleLabel;
    std::unique_ptr<AdsrVisualizer> visualizer;

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdsrComponent)
};
