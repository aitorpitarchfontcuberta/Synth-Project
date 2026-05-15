/*
  ==============================================================================

    AdsrVisualizer.cpp

  ==============================================================================
*/

#include "AdsrVisualizer.h"

AdsrVisualizer::AdsrVisualizer(juce::AudioProcessorValueTreeState& apvts,
                               juce::String attackID,
                               juce::String decayID,
                               juce::String sustainID,
                               juce::String releaseID,
                               juce::Colour curveColour)
    : apvtsRef(apvts),
      aID(std::move(attackID)),
      dID(std::move(decayID)),
      sID(std::move(sustainID)),
      rID(std::move(releaseID)),
      colour(curveColour)
{
    startTimerHz(30);
}

AdsrVisualizer::~AdsrVisualizer()
{
    stopTimer();
}

void AdsrVisualizer::timerCallback()
{
    const float a = apvtsRef.getRawParameterValue(aID)->load();
    const float d = apvtsRef.getRawParameterValue(dID)->load();
    const float s = apvtsRef.getRawParameterValue(sID)->load();
    const float r = apvtsRef.getRawParameterValue(rID)->load();

    if (a != lastA || d != lastD || s != lastS || r != lastR)
    {
        lastA = a; lastD = d; lastS = s; lastR = r;
        repaint();
    }
}

void AdsrVisualizer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    g.setColour(juce::Colour(0xff10101a));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a50));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    g.setColour(juce::Colour(0xff242438));
    for (int i = 1; i < 4; ++i)
    {
        float y = bounds.getY() + bounds.getHeight() * (i / 4.0f);
        g.drawHorizontalLine((int)y, bounds.getX(), bounds.getRight());
    }

    const float a = apvtsRef.getRawParameterValue(aID)->load();
    const float d = apvtsRef.getRawParameterValue(dID)->load();
    const float s = apvtsRef.getRawParameterValue(sID)->load();
    const float r = apvtsRef.getRawParameterValue(rID)->load();

    auto compress = [](float seconds) { return std::sqrt(juce::jlimit(0.0f, 5.0f, seconds)); };

    const float aLen = compress(a), dLen = compress(d), rLen = compress(r);
    const float sustainHoldLen = std::sqrt(1.5f);
    const float totalLen = aLen + dLen + sustainHoldLen + rLen;

    const float padding = 6.0f;
    const float plotX = bounds.getX() + padding;
    const float plotY = bounds.getY() + padding;
    const float plotW = bounds.getWidth()  - padding * 2.0f;
    const float plotH = bounds.getHeight() - padding * 2.0f;

    auto xAt = [&](float accumLen) { return plotX + (accumLen / totalLen) * plotW; };
    auto yAt = [&](float level)    { return plotY + (1.0f - level) * plotH; };

    const float xStart   = xAt(0.0f);
    const float xPeak    = xAt(aLen);
    const float xSustain = xAt(aLen + dLen);
    const float xRelease = xAt(aLen + dLen + sustainHoldLen);
    const float xEnd     = xAt(aLen + dLen + sustainHoldLen + rLen);

    const float yBase    = yAt(0.0f);
    const float yPeak    = yAt(1.0f);
    const float ySustain = yAt(s);

    juce::Path env;
    env.startNewSubPath(xStart, yBase);
    env.lineTo(xPeak,    yPeak);
    env.lineTo(xSustain, ySustain);
    env.lineTo(xRelease, ySustain);
    env.lineTo(xEnd,     yBase);

    juce::Path filled = env;
    filled.lineTo(xEnd,   yBase);
    filled.lineTo(xStart, yBase);
    filled.closeSubPath();

    g.setColour(colour.withAlpha(0.15f));
    g.fillPath(filled);

    g.setColour(colour);
    g.strokePath(env, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    auto drawNode = [&](float x, float y)
    {
        g.setColour(juce::Colours::white);
        g.fillEllipse(x - 2.5f, y - 2.5f, 5.0f, 5.0f);
    };
    drawNode(xStart, yBase);
    drawNode(xPeak,  yPeak);
    drawNode(xSustain, ySustain);
    drawNode(xRelease, ySustain);
    drawNode(xEnd,    yBase);
}
