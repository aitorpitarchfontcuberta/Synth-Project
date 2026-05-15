/*
  ==============================================================================

    PresetBarComponent.h
    Author: Aitor

    Barra superior de gestion de presets: < | Nombre del preset | > | Save
    - Click izquierdo sobre el nombre: menu de seleccion.
    - Click derecho sobre el nombre (si es user preset): Edit name / Delete.
    - Asterisco al final del nombre si el estado actual difiere del cargado.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Data/PresetManager.h"

class PresetBarComponent : public juce::Component,
                           public juce::MouseListener
{
public:
    explicit PresetBarComponent(PresetManager& manager);
    ~PresetBarComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // MouseListener
    void mouseDown(const juce::MouseEvent& e) override;

private:
    void showPresetMenu();
    void showContextMenuForCurrentPreset();
    void doSavePreset();
    void doRenameCurrentPreset();
    void doDeleteCurrentPreset();

    void refreshNameButton();

    PresetManager& manager;

    juce::TextButton nameBtn;
    juce::TextButton saveBtn { "Save" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBarComponent)
};
