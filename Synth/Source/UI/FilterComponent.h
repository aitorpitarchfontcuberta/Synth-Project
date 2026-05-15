/*
  ==============================================================================

    FilterComponent.h
    Author: Aitor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FilterResponseVisualizer.h"
#include "../Data/ScopeBuffer.h"

class FilterComponent : public juce::Component
{
public:
    FilterComponent(juce::AudioProcessorValueTreeState& apvts,
                    const ScopeBuffer& scopeBuffer);
    ~FilterComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParams(juce::Slider& s);
    void setLabelParams(juce::Label& l, const juce::String& text);

    juce::Label titleLabel;
    std::unique_ptr<FilterResponseVisualizer> responseVis;

    juce::Label    typeLabel;
    juce::ComboBox typeBox;

    juce::Slider cutoffSlider, resSlider, driveSlider, keyTrackSlider;
    juce::Label  cutoffLabel,  resLabel,  driveLabel,  keyTrackLabel;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<CA> typeA;
    std::unique_ptr<SA> cutoffA, resA, driveA, keyTrackA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterComponent)
};
