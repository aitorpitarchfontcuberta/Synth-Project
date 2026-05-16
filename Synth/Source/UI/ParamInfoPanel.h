/*
  ==============================================================================

    ParamInfoPanel.h
    Author: Aitor

    Panel popup que muestra informacion didactica de un parametro: nombre,
    categoria, descripcion y efectos al subir/bajar. Aparece flotando dentro
    del editor cuando se hace click derecho sobre un knob/combo.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Data/ParamInfoDatabase.h"

class ParamInfoPanel : public juce::Component
{
public:
    ParamInfoPanel();
    ~ParamInfoPanel() override;

    void setInfo(const ParamInfo& info);

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

    std::function<void()> onClose;

private:
    juce::Label categoryLabel;
    juce::Label titleLabel;

    juce::Label descriptionLabel;
    juce::Label upHeaderLabel;
    juce::Label upLabel;
    juce::Label downHeaderLabel;
    juce::Label downLabel;
    juce::Label tipsHeaderLabel;
    juce::Label tipsLabel;

    juce::TextButton closeBtn { "X" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParamInfoPanel)
};
