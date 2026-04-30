/*
  ==============================================================================

    AdsrData.cpp
    Created: 30 Apr 2026 11:36:41am
    Author:  Aitor

  ==============================================================================
*/

#include "AdsrData.h"

void AdsrData::updateADSR(const float attack, const float decay, const float sustain, const float release)
{
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;

    setParameters(adsrParams);
}