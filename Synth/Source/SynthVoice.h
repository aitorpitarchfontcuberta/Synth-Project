/*
  ==============================================================================

    SynthVoice.h
    Created: 23 Apr 2026 2:12:30pm
    Author:  aitor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "Data/AdsrData.h"
#include "Data/OscData.h"
#include "Data/LfoData.h"
#include "Data/FilterData.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    enum OscMode    { ModeMix = 0, ModeRing, ModeSync, ModeFM };
    enum ModDest    { DestNone = 0, DestPitch, DestOsc1PW, DestOsc2PW, DestAmp, DestMix, DestFmAmt, DestCutoff, DestRes };
    enum VoiceMode  { Poly = 0, Mono, Legato };

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    // Parametros leidos en bloque, almacenados en la voz hasta el proximo bloque
    void setAmpAdsr      (float a, float d, float s, float r);
    void setModAdsr      (float a, float d, float s, float r);
    void setModEnv       (int destination, float depth);
    void setOscParams    (int wave1, int oct1, int semi1, float fine1, float pw1,
                          int wave2, int oct2, int semi2, float fine2, float pw2);
    void setSubParams    (int wave, float level);
    void setMixAndMode   (float mix, int mode, float fmAmount);
    void setLfo1         (int wave, float rateHz, float depth, int destination);
    void setLfo2         (int wave, float rateHz, float depth, int destination);
    void setMaster       (float gain, float velocitySensitivity, float glideSeconds, int voiceMode);
    void setUnison       (int voices, float detuneCents, float spread);
    void setFilter       (int type, float cutoff, float resonance, float drive, float keyTrack);

    // Para mono/legato gestionado desde el procesador
    bool isNoteHeld() const { return noteIsHeld; }

private:
    void applyModulationsForSample(float lfo1Val, float lfo2Val, float modEnvVal);

    AdsrData ampAdsr;
    AdsrData modAdsr;

    // Hasta 7 voces unison apiladas por OSC1 y OSC2
    static constexpr int kMaxUnison = 7;
    OscData osc1[kMaxUnison];
    OscData osc2[kMaxUnison];

    OscData sub;
    LfoData lfo1;
    LfoData lfo2;
    FilterData filter;

    juce::AudioBuffer<float> synthBuffer;

    // Parametros cacheados (set desde el processor cada bloque)
    int   wave1 = 0, oct1 = 0, semi1 = 0;
    int   wave2 = 0, oct2 = 0, semi2 = 0;
    float fine1 = 0.0f, pw1 = 0.5f;
    float fine2 = 0.0f, pw2 = 0.5f;

    int   subWave = 0;
    float subLevel = 0.0f;

    float mixAmount = 0.5f;
    int   oscMode = ModeMix;
    float fmAmount = 0.0f;

    int   lfo1Wave = 0, lfo1Dest = DestNone;
    int   lfo2Wave = 0, lfo2Dest = DestNone;
    float lfo1Rate = 1.0f, lfo1Depth = 0.0f;
    float lfo2Rate = 1.0f, lfo2Depth = 0.0f;

    int   modEnvDest = DestNone;
    float modEnvDepth = 0.0f;

    float masterGain = 0.7f;
    float velocitySens = 0.5f;
    float glideTimeSeconds = 0.0f;
    int   voiceMode = Poly;

    int   unisonVoices = 1;
    float unisonDetune = 0.0f;
    float unisonSpread = 0.0f;

    // Filtro
    int   filterType = 0;
    float filterCutoff = 8000.0f;
    float filterResonance = 0.7f;
    float filterDrive = 1.0f;
    float filterKeyTrack = 0.0f;

    float currentVelocity = 1.0f;
    int   currentMidiNote = 60;

    // Glide: suavizado de pitch entre notas
    juce::SmoothedValue<float> glidePitchSemis { 0.0f };

    // Pitch wheel: -2..+2 semitonos
    float pitchBendSemis = 0.0f;

    bool noteIsHeld = false;
    bool isPrepared = false;
    double currentSampleRate = 44100.0;
};
