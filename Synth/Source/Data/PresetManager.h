/*
  ==============================================================================

    PresetManager.h
    Author: Aitor

    Sistema de presets: gestiona el banco de fabrica (hardcoded en
    FactoryPresets) y los presets de usuario (en disco como XML).

    Tambien rastrea si el estado actual difiere del preset cargado (modified)
    a traves de un APVTS::Listener registrado a todos los parametros.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <atomic>
#include "FactoryPresets.h"

class PresetManager : private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit PresetManager(juce::AudioProcessorValueTreeState& apvts);
    ~PresetManager() override;

    // Carga un preset por indice de la lista combinada (factory + user).
    bool loadPreset(int index);

    // Guarda el estado actual como preset de usuario con el nombre dado.
    bool saveUserPreset(const juce::String& name);

    // Renombrar y eliminar (solo para user presets).
    bool renameUserPreset(int index, const juce::String& newName);
    bool deleteUserPreset(int index);

    // Navegacion entre presets
    void nextPreset();
    void prevPreset();

    int          getNumPresets()         const { return (int)entries.size(); }
    int          getCurrentPresetIndex() const { return currentIndex; }
    juce::String getCurrentPresetName()  const;

    // True si el estado actual del APVTS difiere del preset cargado.
    bool isModified() const { return dirty.load(); }

    // Para construir el menu
    struct PresetInfo
    {
        juce::String name;
        juce::String category;
        bool         isFactory;
    };
    PresetInfo getPresetInfo(int index) const;

    // Refresca la lista de presets de usuario desde disco
    void rescanUserPresets();

    // Callback opcional cuando el preset actual cambia o el estado dirty cambia.
    std::function<void()> onPresetChanged;

private:
    // APVTS::Listener
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    struct Entry
    {
        juce::String name;
        juce::String category;
        bool         isFactory;
        int          factoryIndex; // -1 si es user
        juce::File   file;          // valido si es user
    };

    void rebuildEntries();
    void registerAsListener();
    void unregisterAsListener();
    juce::File getUserPresetsDir() const;

    juce::AudioProcessorValueTreeState& apvtsRef;
    std::vector<Entry>        entries;
    std::vector<juce::String> listenedIDs;

    int currentIndex = -1;

    // loadingPreset evita que el listener marque dirty durante la aplicacion
    // de un preset (donde muchos parametros cambian secuencialmente).
    std::atomic<bool> loadingPreset { false };
    std::atomic<bool> dirty         { false };
};
