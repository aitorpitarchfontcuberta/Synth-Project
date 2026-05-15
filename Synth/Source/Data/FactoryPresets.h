/*
  ==============================================================================

    FactoryPresets.h
    Author: Aitor

    Banco de presets de fabrica. Cada preset es un mapa de parametros
    (id del APVTS -> valor) que se aplica al cargarlo.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <map>

struct FactoryPreset
{
    juce::String name;
    juce::String category;
    std::map<juce::String, float> params;
};

namespace FactoryPresets {

// Lista de presets disponibles. El primero es siempre "Init".
const std::vector<FactoryPreset>& getAll();

// Aplica los parametros del preset al APVTS.
void apply(const FactoryPreset& preset, juce::AudioProcessorValueTreeState& apvts);

} // namespace FactoryPresets
