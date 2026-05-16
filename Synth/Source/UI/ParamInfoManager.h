/*
  ==============================================================================

    ParamInfoManager.h
    Author: Aitor

    Gestor del sistema de informacion de parametros. Cada slider/combo se
    registra via attachTo(). Cuando recibe un click derecho, abre un
    ParamInfoPanel flotante encima del componente raiz indicado.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ParamInfoPanel.h"

class ParamInfoManager : public juce::MouseListener
{
public:
    ParamInfoManager();
    ~ParamInfoManager() override;

    // Indica el componente que servira de contenedor para el panel popup.
    void setParentForPanel(juce::Component* parent);

    // Registra un componente (Slider, ComboBox...) con su paramID.
    void attachTo(juce::Component& target, const juce::String& paramID);

    // Cierra el panel si esta abierto.
    void closePanel();

private:
    // MouseListener
    void mouseDown(const juce::MouseEvent& e) override;

    void showPanelFor(const juce::String& paramID, const juce::MouseEvent& sourceEvent);

    juce::Component* parentForPanel = nullptr;
    std::unique_ptr<ParamInfoPanel> currentPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParamInfoManager)
};
