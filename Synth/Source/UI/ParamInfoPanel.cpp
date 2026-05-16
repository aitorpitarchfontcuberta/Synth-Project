/*
  ==============================================================================

    ParamInfoPanel.cpp

  ==============================================================================
*/

#include "ParamInfoPanel.h"

ParamInfoPanel::ParamInfoPanel()
{
    auto setupHeader = [&](juce::Label& l, const juce::String& text, juce::Colour col, float fontSize, bool bold)
    {
        l.setText(text, juce::dontSendNotification);
        l.setFont(juce::Font(fontSize, bold ? juce::Font::bold : 0));
        l.setColour(juce::Label::textColourId, col);
        l.setJustificationType(juce::Justification::topLeft);
        addAndMakeVisible(l);
    };
    auto setupBody = [&](juce::Label& l)
    {
        l.setFont(juce::Font(12.0f));
        l.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        l.setJustificationType(juce::Justification::topLeft);
        l.setMinimumHorizontalScale(1.0f);
        addAndMakeVisible(l);
    };

    setupHeader(categoryLabel,    "",        juce::Colour(0xff7a9dff),  10.5f, true);
    setupHeader(titleLabel,       "",        juce::Colours::white,      18.0f, true);
    setupBody  (descriptionLabel);
    setupHeader(upHeaderLabel,    "Al subir",   juce::Colour(0xff8effc1), 11.5f, true);
    setupBody  (upLabel);
    setupHeader(downHeaderLabel,  "Al bajar",   juce::Colour(0xffffaa55), 11.5f, true);
    setupBody  (downLabel);
    setupHeader(tipsHeaderLabel,  "Consejos",   juce::Colour(0xfffff066), 11.5f, true);
    setupBody  (tipsLabel);

    closeBtn.setColour(juce::TextButton::buttonColourId,  juce::Colour(0xff2a2a3a));
    closeBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    closeBtn.onClick = [this]
    {
        if (onClose) onClose();
    };
    addAndMakeVisible(closeBtn);
}

ParamInfoPanel::~ParamInfoPanel() {}

void ParamInfoPanel::setInfo(const ParamInfo& info)
{
    categoryLabel.setText(info.category.toUpperCase(), juce::dontSendNotification);
    titleLabel   .setText(info.displayName, juce::dontSendNotification);
    descriptionLabel.setText(info.description, juce::dontSendNotification);
    upLabel  .setText(info.effectUp,   juce::dontSendNotification);
    downLabel.setText(info.effectDown, juce::dontSendNotification);
    tipsLabel.setText(info.tips,       juce::dontSendNotification);

    // Si no hay un efecto "Al bajar" (en parametros tipo Selector), ocultamos la seccion
    const bool hasDown = info.effectDown.isNotEmpty();
    downHeaderLabel.setVisible(hasDown);
    downLabel      .setVisible(hasDown);

    const bool hasTips = info.tips.isNotEmpty();
    tipsHeaderLabel.setVisible(hasTips);
    tipsLabel      .setVisible(hasTips);

    resized();
}

void ParamInfoPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    // Sombra externa
    juce::DropShadow shadow(juce::Colours::black.withAlpha(0.6f), 12, { 0, 4 });
    juce::Path shadowPath;
    shadowPath.addRoundedRectangle(bounds, 10.0f);
    shadow.drawForPath(g, shadowPath);

    // Fondo del panel
    juce::ColourGradient bg(juce::Colour(0xff1f1f2e), bounds.getTopLeft(),
                            juce::Colour(0xff15151f), bounds.getBottomRight(), false);
    g.setGradientFill(bg);
    g.fillRoundedRectangle(bounds, 10.0f);

    // Borde
    g.setColour(juce::Colour(0xff7a9dff).withAlpha(0.6f));
    g.drawRoundedRectangle(bounds, 10.0f, 1.5f);
}

void ParamInfoPanel::resized()
{
    auto b = getLocalBounds().reduced(14);

    // Boton cerrar arriba derecha
    closeBtn.setBounds(getWidth() - 30, 6, 24, 22);

    // Cabecera: categoria + titulo
    categoryLabel.setBounds(b.removeFromTop(14));
    titleLabel   .setBounds(b.removeFromTop(24));

    b.removeFromTop(6);

    auto layoutTextSection = [&](juce::Label& header, juce::Label& body,
                                 int bodyHeight, bool drawHeader)
    {
        if (drawHeader)
            header.setBounds(b.removeFromTop(16));
        body.setBounds(b.removeFromTop(bodyHeight));
        b.removeFromTop(6);
    };

    // Descripcion (con altura adaptable)
    descriptionLabel.setBounds(b.removeFromTop(48));
    b.removeFromTop(8);

    if (upLabel.getText().isNotEmpty())
        layoutTextSection(upHeaderLabel, upLabel, 60, true);

    if (downLabel.isVisible())
        layoutTextSection(downHeaderLabel, downLabel, 60, true);

    if (tipsLabel.isVisible())
        layoutTextSection(tipsHeaderLabel, tipsLabel, 60, true);
}

void ParamInfoPanel::mouseDown(const juce::MouseEvent&)
{
    // Capturamos clicks dentro del panel para no propagarlos al fondo.
}
