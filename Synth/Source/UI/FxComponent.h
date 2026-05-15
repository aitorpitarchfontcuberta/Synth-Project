/*
  ==============================================================================

    FxComponent.h
    Author: Aitor

    Pestania de efectos: Chorus + Delay + Reverb apilados verticalmente,
    cada uno con un mini visualizador didactico de su comportamiento.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FxComponent : public juce::Component
{
public:
    explicit FxComponent(juce::AudioProcessorValueTreeState& apvts);
    ~FxComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    class FxBlock; // forward
    std::unique_ptr<FxBlock> chorusBlock;
    std::unique_ptr<FxBlock> delayBlock;
    std::unique_ptr<FxBlock> reverbBlock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxComponent)
};
