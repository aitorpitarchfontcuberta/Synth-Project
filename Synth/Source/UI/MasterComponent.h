/*
  ==============================================================================

    MasterComponent.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "UnisonVisualizer.h"
#include "VelocityCurveVisualizer.h"
#include "GlideVisualizer.h"

class MasterComponent : public juce::Component
{
public:
    MasterComponent(juce::AudioProcessorValueTreeState& apvts);
    ~MasterComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParamsCompact(juce::Slider& s); // Para los knobs de la fila inferior (textbox abajo)
    void setSliderParamsSide   (juce::Slider& s); // Para Vel/Glide (textbox a la izquierda)
    void setLabelParams        (juce::Label& l, const juce::String& t);

    juce::Label titleLabel;
    std::unique_ptr<VelocityCurveVisualizer> velVis;
    std::unique_ptr<GlideVisualizer> glideVis;
    std::unique_ptr<UnisonVisualizer> unisonVis;

    juce::Slider gainSlider, velSlider, glideSlider, unisonSlider, detuneSlider, spreadSlider;
    juce::Label  gainLabel,  velLabel,  glideLabel,  unisonLabel,  detuneLabel,  spreadLabel;

    juce::Label voiceModeLabel;
    juce::ComboBox voiceModeBox;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SA> gainA, velA, glideA, unisonA, detuneA, spreadA;
    std::unique_ptr<CA> voiceModeA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterComponent)
};
