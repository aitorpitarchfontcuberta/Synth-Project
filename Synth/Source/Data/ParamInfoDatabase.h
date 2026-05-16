/*
  ==============================================================================

    ParamInfoDatabase.h
    Author: Aitor

    Base de datos de descripciones didacticas para cada parametro. Se consulta
    mediante getInfo(paramID), con normalizacion automatica de prefijos
    (OSC1/OSC2 -> OSC, LFO1/LFO2 -> LFO) cuando la descripcion es comun.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ParamInfo
{
    juce::String displayName;   // ej: "Filter Cutoff"
    juce::String category;      // ej: "Filter"
    juce::String description;   // que hace el parametro
    juce::String effectUp;      // efecto al subir
    juce::String effectDown;    // efecto al bajar
    juce::String tips;          // consejos / casos de uso

    bool isValid() const { return displayName.isNotEmpty(); }
};

namespace ParamInfoDatabase {

// Devuelve la info de un parametro por su ID del APVTS. Si no se encuentra,
// devuelve un ParamInfo con displayName vacio (.isValid() == false).
ParamInfo getInfo(const juce::String& paramID);

} // namespace ParamInfoDatabase
