/*
  ==============================================================================

    PresetBarComponent.cpp

  ==============================================================================
*/

#include "PresetBarComponent.h"

PresetBarComponent::PresetBarComponent(PresetManager& m)
    : manager(m)
{
    auto styleButton = [](juce::TextButton& b)
    {
        b.setColour(juce::TextButton::buttonColourId,   juce::Colour(0xff2a2a3a));
        b.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff3a3a55));
        b.setColour(juce::TextButton::textColourOffId,  juce::Colours::white);
        b.setColour(juce::TextButton::textColourOnId,   juce::Colours::white);
    };
    styleButton(nameBtn);
    styleButton(saveBtn);

    nameBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1a1a25));
    nameBtn.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff8effc1));

    addAndMakeVisible(nameBtn);
    addAndMakeVisible(saveBtn);

    nameBtn.onClick = [this] { showPresetMenu(); };
    saveBtn.onClick = [this] { doSavePreset(); };

    // Capturar click derecho sobre el nameBtn
    nameBtn.addMouseListener(this, false);

    manager.onPresetChanged = [this] { refreshNameButton(); };
    refreshNameButton();
}

PresetBarComponent::~PresetBarComponent()
{
    nameBtn.removeMouseListener(this);
    manager.onPresetChanged = nullptr;
}

void PresetBarComponent::refreshNameButton()
{
    auto name = manager.getCurrentPresetName();
    if (manager.isModified())
        name += " *";
    nameBtn.setButtonText(name);
    repaint();
}

void PresetBarComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    juce::ColourGradient grad(juce::Colour(0xff1a1a25), bounds.getTopLeft(),
                              juce::Colour(0xff10101a), bounds.getBottomRight(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    g.setColour(juce::Colours::lightgrey);
    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.drawText("PRESET", bounds.toNearestInt().reduced(8, 0),
               juce::Justification::centredLeft);
}

void PresetBarComponent::resized()
{
    auto b = getLocalBounds().reduced(8, 4);
    b.removeFromLeft(60); // hueco para la etiqueta "PRESET"

    const int saveW = 60;
    const int gap   = 6;

    saveBtn.setBounds(b.removeFromRight(saveW));
    b.removeFromRight(gap);

    nameBtn.setBounds(b);
}

void PresetBarComponent::mouseDown(const juce::MouseEvent& e)
{
    if (e.eventComponent == &nameBtn && e.mods.isPopupMenu())
    {
        showContextMenuForCurrentPreset();
    }
}

void PresetBarComponent::showPresetMenu()
{
    juce::PopupMenu menu;

    juce::String currentCategory;
    juce::PopupMenu submenu;

    auto flushSubmenu = [&]()
    {
        if (currentCategory.isNotEmpty() && submenu.getNumItems() > 0)
            menu.addSubMenu(currentCategory, submenu);
        submenu.clear();
    };

    const int n = manager.getNumPresets();
    for (int i = 0; i < n; ++i)
    {
        auto info = manager.getPresetInfo(i);
        if (info.category != currentCategory)
        {
            flushSubmenu();
            currentCategory = info.category;
        }
        submenu.addItem(i + 1, info.name, true, i == manager.getCurrentPresetIndex());
    }
    flushSubmenu();

    menu.showMenuAsync(juce::PopupMenu::Options()
                         .withTargetComponent(&nameBtn),
                       [this](int result)
                       {
                           if (result > 0)
                               manager.loadPreset(result - 1);
                       });
}

void PresetBarComponent::showContextMenuForCurrentPreset()
{
    const int idx = manager.getCurrentPresetIndex();
    if (idx < 0) return;

    auto info = manager.getPresetInfo(idx);
    if (info.isFactory)
    {
        // Para presets de fabrica solo mostramos un mensaje informativo
        juce::PopupMenu menu;
        menu.addSectionHeader("Factory preset");
        menu.addItem(1, "(read-only)", false);
        menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&nameBtn));
        return;
    }

    juce::PopupMenu menu;
    menu.addSectionHeader(info.name);
    menu.addItem(1, "Edit name...");
    menu.addItem(2, "Delete preset");

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&nameBtn),
                       [this](int result)
                       {
                           if (result == 1) doRenameCurrentPreset();
                           else if (result == 2) doDeleteCurrentPreset();
                       });
}

void PresetBarComponent::doSavePreset()
{
    auto alert = std::make_shared<juce::AlertWindow>(
        "Save Preset",
        "Enter a name for the preset:",
        juce::MessageBoxIconType::QuestionIcon);

    alert->addTextEditor("name", "");
    alert->addButton("Save",   1, juce::KeyPress(juce::KeyPress::returnKey));
    alert->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    auto callback = juce::ModalCallbackFunction::create(
        [this, alert](int result)
        {
            if (result == 1)
            {
                auto name = alert->getTextEditorContents("name");
                if (name.trim().isNotEmpty())
                    manager.saveUserPreset(name);
            }
        });

    alert->enterModalState(true, callback, false);
}

void PresetBarComponent::doRenameCurrentPreset()
{
    const int idx = manager.getCurrentPresetIndex();
    if (idx < 0) return;

    auto current = manager.getPresetInfo(idx);
    if (current.isFactory) return;

    auto alert = std::make_shared<juce::AlertWindow>(
        "Rename Preset",
        "Enter a new name:",
        juce::MessageBoxIconType::QuestionIcon);

    alert->addTextEditor("name", current.name);
    alert->addButton("Rename", 1, juce::KeyPress(juce::KeyPress::returnKey));
    alert->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    auto callback = juce::ModalCallbackFunction::create(
        [this, alert, idx](int result)
        {
            if (result == 1)
            {
                auto newName = alert->getTextEditorContents("name");
                if (newName.trim().isNotEmpty())
                {
                    if (! manager.renameUserPreset(idx, newName))
                    {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::MessageBoxIconType::WarningIcon,
                            "Rename failed",
                            "Could not rename the preset. Maybe another preset with that name already exists?");
                    }
                }
            }
        });

    alert->enterModalState(true, callback, false);
}

void PresetBarComponent::doDeleteCurrentPreset()
{
    const int idx = manager.getCurrentPresetIndex();
    if (idx < 0) return;

    auto current = manager.getPresetInfo(idx);
    if (current.isFactory) return;

    juce::AlertWindow::showOkCancelBox(
        juce::MessageBoxIconType::QuestionIcon,
        "Delete preset",
        "Delete '" + current.name + "'? This cannot be undone.",
        "Delete", "Cancel",
        nullptr,
        juce::ModalCallbackFunction::create(
            [this, idx](int result)
            {
                if (result == 1) // OK
                    manager.deleteUserPreset(idx);
            }));
}
