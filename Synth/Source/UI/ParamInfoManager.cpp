/*
  ==============================================================================

    ParamInfoManager.cpp

  ==============================================================================
*/

#include "ParamInfoManager.h"
#include "../Data/ParamInfoDatabase.h"

ParamInfoManager::ParamInfoManager() {}

ParamInfoManager::~ParamInfoManager()
{
    closePanel();
}

void ParamInfoManager::setParentForPanel(juce::Component* parent)
{
    parentForPanel = parent;
}

void ParamInfoManager::attachTo(juce::Component& target, const juce::String& paramID)
{
    target.setComponentID(paramID);
    target.addMouseListener(this, false);
}

void ParamInfoManager::closePanel()
{
    currentPanel.reset();
}

void ParamInfoManager::mouseDown(const juce::MouseEvent& e)
{
    if (! e.mods.isPopupMenu()) return;

    auto* src = e.eventComponent;
    if (src == nullptr) return;

    auto paramID = src->getComponentID();
    if (paramID.isEmpty()) return;

    showPanelFor(paramID, e);
}

void ParamInfoManager::showPanelFor(const juce::String& paramID, const juce::MouseEvent& sourceEvent)
{
    if (parentForPanel == nullptr) return;

    auto info = ParamInfoDatabase::getInfo(paramID);
    if (! info.isValid())
    {
        // Si no tenemos info, mostramos un panel minimo para no dejar al usuario sin feedback.
        info.displayName = paramID;
        info.category    = "Parameter";
        info.description = "Sin descripcion disponible para este parametro todavia.";
    }

    // Crear (o sustituir) el panel
    currentPanel = std::make_unique<ParamInfoPanel>();
    currentPanel->setInfo(info);
    currentPanel->onClose = [this] { closePanel(); };

    parentForPanel->addAndMakeVisible(*currentPanel);
    currentPanel->toFront(true);

    // Posicionar el panel: a la derecha del origen del click, o ajustado si se sale.
    const int panelW = 360;
    const int panelH = 340;

    auto sourceGlobal = sourceEvent.eventComponent->getScreenBounds();
    auto parentGlobal = parentForPanel->getScreenBounds();
    auto sourceInParent = sourceGlobal.translated(-parentGlobal.getX(), -parentGlobal.getY());

    int x = sourceInParent.getRight() + 8;
    int y = sourceInParent.getY();

    // Ajustar dentro de los limites del parent
    if (x + panelW > parentForPanel->getWidth())
        x = sourceInParent.getX() - panelW - 8;
    if (x < 4)
        x = 4;
    if (y + panelH > parentForPanel->getHeight())
        y = parentForPanel->getHeight() - panelH - 4;
    if (y < 4)
        y = 4;

    currentPanel->setBounds(x, y, panelW, panelH);
}
