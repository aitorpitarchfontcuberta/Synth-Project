/*
  ==============================================================================

    PresetManager.cpp

  ==============================================================================
*/

#include "PresetManager.h"

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    rebuildEntries();
    registerAsListener();
}

PresetManager::~PresetManager()
{
    unregisterAsListener();
}

void PresetManager::registerAsListener()
{
    listenedIDs.clear();
    // Iteramos los hijos del state del APVTS para descubrir todos los IDs de
    // parametro y registrarnos a cada uno. Asi cualquier cambio (de la UI,
    // del DAW o de un preset load) lo capturamos.
    for (int i = 0; i < apvtsRef.state.getNumChildren(); ++i)
    {
        auto child = apvtsRef.state.getChild(i);
        if (child.hasType("PARAM"))
        {
            auto id = child.getProperty("id").toString();
            if (id.isNotEmpty())
            {
                apvtsRef.addParameterListener(id, this);
                listenedIDs.push_back(id);
            }
        }
    }
}

void PresetManager::unregisterAsListener()
{
    for (auto& id : listenedIDs)
        apvtsRef.removeParameterListener(id, this);
    listenedIDs.clear();
}

void PresetManager::parameterChanged(const juce::String&, float)
{
    if (loadingPreset.load()) return;
    if (! dirty.load())
    {
        dirty.store(true);
        if (onPresetChanged)
            juce::MessageManager::callAsync([cb = onPresetChanged] { cb(); });
    }
}

juce::File PresetManager::getUserPresetsDir() const
{
    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("AitorPitarchFontcuberta")
                   .getChildFile("Synth")
                   .getChildFile("Presets");
    if (!dir.exists())
        dir.createDirectory();
    return dir;
}

void PresetManager::rebuildEntries()
{
    entries.clear();

    const auto& factory = FactoryPresets::getAll();
    for (size_t i = 0; i < factory.size(); ++i)
    {
        Entry e;
        e.name         = factory[i].name;
        e.category     = factory[i].category;
        e.isFactory    = true;
        e.factoryIndex = (int) i;
        entries.push_back(e);
    }

    auto dir = getUserPresetsDir();
    juce::Array<juce::File> files;
    dir.findChildFiles(files, juce::File::findFiles, false, "*.xml");
    files.sort();
    for (auto& f : files)
    {
        Entry e;
        e.name         = f.getFileNameWithoutExtension();
        e.category     = "User";
        e.isFactory    = false;
        e.factoryIndex = -1;
        e.file         = f;
        entries.push_back(e);
    }
}

void PresetManager::rescanUserPresets()
{
    rebuildEntries();
    if (onPresetChanged) onPresetChanged();
}

bool PresetManager::loadPreset(int index)
{
    if (index < 0 || index >= (int) entries.size())
        return false;

    const auto& e = entries[(size_t) index];

    loadingPreset.store(true);

    bool success = false;
    if (e.isFactory)
    {
        const auto& factory = FactoryPresets::getAll();
        if (e.factoryIndex >= 0 && e.factoryIndex < (int) factory.size())
        {
            FactoryPresets::apply(factory[(size_t) e.factoryIndex], apvtsRef);
            success = true;
        }
    }
    else
    {
        if (e.file.existsAsFile())
        {
            auto xml = juce::XmlDocument::parse(e.file);
            if (xml != nullptr && xml->hasTagName(apvtsRef.state.getType().toString()))
            {
                apvtsRef.replaceState(juce::ValueTree::fromXml(*xml));
                success = true;
            }
        }
    }

    // Esperamos un instante para que los listeners diferidos terminen, y
    // limpiamos el flag de "modificado" usando callAsync. Hacerlo asincrono
    // garantiza que el flag se ponga a false despues de que cualquier
    // parameterChanged pendiente haya pasado.
    juce::MessageManager::callAsync([this]
    {
        loadingPreset.store(false);
        dirty.store(false);
        if (onPresetChanged) onPresetChanged();
    });

    if (success)
        currentIndex = index;

    return success;
}

bool PresetManager::saveUserPreset(const juce::String& nameRaw)
{
    auto name = nameRaw.trim();
    if (name.isEmpty()) return false;

    auto safeName = juce::File::createLegalFileName(name);
    auto file = getUserPresetsDir().getChildFile(safeName + ".xml");

    auto state = apvtsRef.copyState();
    auto xml = state.createXml();
    if (xml == nullptr) return false;

    if (! xml->writeTo(file)) return false;

    rebuildEntries();

    for (size_t i = 0; i < entries.size(); ++i)
    {
        if (! entries[i].isFactory && entries[i].file == file)
        {
            currentIndex = (int) i;
            break;
        }
    }

    dirty.store(false);
    if (onPresetChanged) onPresetChanged();
    return true;
}

bool PresetManager::renameUserPreset(int index, const juce::String& newNameRaw)
{
    if (index < 0 || index >= (int) entries.size()) return false;
    auto& e = entries[(size_t) index];
    if (e.isFactory) return false;
    if (! e.file.existsAsFile()) return false;

    auto newName = newNameRaw.trim();
    if (newName.isEmpty()) return false;
    auto safeName = juce::File::createLegalFileName(newName);

    auto newFile = getUserPresetsDir().getChildFile(safeName + ".xml");
    if (newFile == e.file) return true; // sin cambio

    // No sobrescribir otro preset existente.
    if (newFile.existsAsFile()) return false;

    if (! e.file.moveFileTo(newFile)) return false;

    const bool wasCurrent = (index == currentIndex);
    rebuildEntries();

    if (wasCurrent)
    {
        for (size_t i = 0; i < entries.size(); ++i)
        {
            if (! entries[i].isFactory && entries[i].file == newFile)
            {
                currentIndex = (int) i;
                break;
            }
        }
    }

    if (onPresetChanged) onPresetChanged();
    return true;
}

bool PresetManager::deleteUserPreset(int index)
{
    if (index < 0 || index >= (int) entries.size()) return false;
    auto& e = entries[(size_t) index];
    if (e.isFactory) return false;

    if (e.file.existsAsFile())
    {
        if (! e.file.deleteFile()) return false;
    }

    const bool wasCurrent = (index == currentIndex);
    rebuildEntries();

    if (wasCurrent)
    {
        // Caemos al primer preset (Init)
        currentIndex = -1;
        if (! entries.empty())
            loadPreset(0);
    }
    else if (currentIndex > index)
    {
        currentIndex -= 1;
    }

    if (onPresetChanged) onPresetChanged();
    return true;
}

void PresetManager::nextPreset()
{
    if (entries.empty()) return;
    int idx = currentIndex < 0 ? 0 : (currentIndex + 1) % (int) entries.size();
    loadPreset(idx);
}

void PresetManager::prevPreset()
{
    if (entries.empty()) return;
    int idx = currentIndex <= 0
              ? (int) entries.size() - 1
              : currentIndex - 1;
    loadPreset(idx);
}

juce::String PresetManager::getCurrentPresetName() const
{
    if (currentIndex < 0 || currentIndex >= (int) entries.size())
        return "(no preset)";
    return entries[(size_t) currentIndex].name;
}

PresetManager::PresetInfo PresetManager::getPresetInfo(int index) const
{
    PresetInfo info;
    if (index < 0 || index >= (int) entries.size())
        return info;
    const auto& e = entries[(size_t) index];
    info.name      = e.name;
    info.category  = e.category;
    info.isFactory = e.isFactory;
    return info;
}
