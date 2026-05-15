/*
  ==============================================================================

    SynthVoice.cpp
    Created: 23 Apr 2026 2:12:30pm
    Author:  aitor

  ==============================================================================
*/

#include "SynthVoice.h"

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    currentMidiNote = midiNoteNumber;
    currentVelocity = velocity;
    noteIsHeld = true;

    pitchWheelMoved(currentPitchWheelPosition);

    // Glide: suaviza desde la frecuencia previa hacia la nueva
    if (glideTimeSeconds > 0.0001f)
    {
        glidePitchSemis.reset(currentSampleRate, (double)glideTimeSeconds);
        glidePitchSemis.setTargetValue((float)midiNoteNumber);
    }
    else
    {
        glidePitchSemis.setCurrentAndTargetValue((float)midiNoteNumber);
    }

    // En Poly siempre reseteamos las envolventes; en Legato solo si no estaba sonando
    const bool retrigger = (voiceMode != Legato) || !ampAdsr.isActive();

    for (int u = 0; u < kMaxUnison; ++u)
    {
        osc1[u].setBaseMidiNote(midiNoteNumber);
        osc2[u].setBaseMidiNote(midiNoteNumber);
        if (retrigger)
        {
            osc1[u].resetPhase();
            osc2[u].resetPhase();
        }
    }
    sub.setBaseMidiNote(midiNoteNumber - 12); // una octava abajo
    if (retrigger) sub.resetPhase();

    if (retrigger)
    {
        ampAdsr.noteOn();
        modAdsr.noteOn();
        lfo1.resetPhase();
        lfo2.resetPhase();
        filter.reset();
    }
}

void SynthVoice::stopNote(float, bool allowTailOff)
{
    noteIsHeld = false;
    ampAdsr.noteOff();
    modAdsr.noteOff();

    if (!allowTailOff || !ampAdsr.isActive())
        clearCurrentNote();
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    // 0..16383, 8192 = centrado. Rango +/- 2 semitonos
    const float norm = ((float)newPitchWheelValue - 8192.0f) / 8192.0f;
    pitchBendSemis = norm * 2.0f;
}

void SynthVoice::controllerMoved(int, int) {}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    currentSampleRate = sampleRate;
    ampAdsr.setSampleRate(sampleRate);
    modAdsr.setSampleRate(sampleRate);

    for (int u = 0; u < kMaxUnison; ++u)
    {
        osc1[u].prepareToPlay(sampleRate);
        osc2[u].prepareToPlay(sampleRate);
    }
    sub.prepareToPlay(sampleRate);
    lfo1.prepareToPlay(sampleRate);
    lfo2.prepareToPlay(sampleRate);
    filter.prepareToPlay(sampleRate);

    juce::ignoreUnused(samplesPerBlock, outputChannels);

    glidePitchSemis.reset(sampleRate, 0.0);

    isPrepared = true;
}

void SynthVoice::setAmpAdsr(float a, float d, float s, float r)
{
    ampAdsr.updateADSR(a, d, s, r);
}

void SynthVoice::setModAdsr(float a, float d, float s, float r)
{
    modAdsr.updateADSR(a, d, s, r);
}

void SynthVoice::setModEnv(int destination, float depth)
{
    modEnvDest  = destination;
    modEnvDepth = depth;
}

void SynthVoice::setOscParams(int w1, int o1, int s1, float f1, float p1,
                              int w2, int o2, int s2, float f2, float p2)
{
    wave1 = w1; oct1 = o1; semi1 = s1; fine1 = f1; pw1 = p1;
    wave2 = w2; oct2 = o2; semi2 = s2; fine2 = f2; pw2 = p2;

    for (int u = 0; u < kMaxUnison; ++u)
    {
        osc1[u].setWaveType(w1);
        osc1[u].setOctave(o1);
        osc1[u].setSemitone(s1);
        osc1[u].setFineTuneCents(f1);
        osc1[u].setPulseWidth(p1);

        osc2[u].setWaveType(w2);
        osc2[u].setOctave(o2);
        osc2[u].setSemitone(s2);
        osc2[u].setFineTuneCents(f2);
        osc2[u].setPulseWidth(p2);
    }
}

void SynthVoice::setSubParams(int wave, float level)
{
    subWave = wave;
    subLevel = level;
    sub.setWaveType(wave);
}

void SynthVoice::setMixAndMode(float mix, int mode, float fm)
{
    mixAmount = mix;
    oscMode = mode;
    fmAmount = fm;
}

void SynthVoice::setLfo1(int wave, float rateHz, float depth, int destination)
{
    lfo1Wave = wave; lfo1Rate = rateHz; lfo1Depth = depth; lfo1Dest = destination;
    lfo1.setWaveType(wave);
    lfo1.setRateHz(rateHz);
}

void SynthVoice::setLfo2(int wave, float rateHz, float depth, int destination)
{
    lfo2Wave = wave; lfo2Rate = rateHz; lfo2Depth = depth; lfo2Dest = destination;
    lfo2.setWaveType(wave);
    lfo2.setRateHz(rateHz);
}

void SynthVoice::setMaster(float gain, float velSens, float glideSecs, int vMode)
{
    masterGain = gain;
    velocitySens = velSens;
    glideTimeSeconds = glideSecs;
    voiceMode = vMode;
}

void SynthVoice::setFilter(int type, float cutoff, float resonance, float drive, float keyTrack)
{
    filterType      = type;
    filterCutoff    = cutoff;
    filterResonance = resonance;
    filterDrive     = drive;
    filterKeyTrack  = keyTrack;
}

void SynthVoice::setUnison(int voices, float detuneCents, float spread)
{
    unisonVoices = juce::jlimit(1, kMaxUnison, voices);
    unisonDetune = detuneCents;
    unisonSpread = juce::jlimit(0.0f, 1.0f, spread);

    // Distribuir detune simetricamente entre las voces de unison
    for (int u = 0; u < kMaxUnison; ++u)
    {
        if (unisonVoices <= 1)
        {
            osc1[u].setDetuneCents(0.0f);
            osc2[u].setDetuneCents(0.0f);
        }
        else
        {
            // Voces de 0 a unisonVoices-1, mapeadas a [-1, +1]
            const float t = (float)u / (float)(unisonVoices - 1) * 2.0f - 1.0f;
            const float cents = t * detuneCents;
            osc1[u].setDetuneCents(cents);
            osc2[u].setDetuneCents(cents);
        }
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    jassert(isPrepared);
    if (!isVoiceActive()) return;

    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();

    const int numChannels = synthBuffer.getNumChannels();

    auto* left  = synthBuffer.getWritePointer(0);
    auto* right = numChannels > 1 ? synthBuffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        // Modulaciones (rango -depth .. +depth aproximadamente)
        const float lfo1Val = lfo1.renderSample() * lfo1Depth;
        const float lfo2Val = lfo2.renderSample() * lfo2Depth;
        const float modEnvNext = modAdsr.getNextSample() * modEnvDepth;

        // Acumular contribuciones de modulacion segun destino
        float pitchModSemis = pitchBendSemis;
        float pwMod1 = 0.0f, pwMod2 = 0.0f;
        float ampMod = 0.0f;
        float mixMod = 0.0f;
        float fmMod  = 0.0f;
        float cutoffMod = 0.0f;
        float resMod    = 0.0f;

        auto applyMod = [&](int dest, float value)
        {
            switch (dest)
            {
                case DestPitch:   pitchModSemis += value * 12.0f; break; // hasta +/- 1 octava
                case DestOsc1PW:  pwMod1 += value * 0.45f; break;
                case DestOsc2PW:  pwMod2 += value * 0.45f; break;
                case DestAmp:     ampMod += value;          break;
                case DestMix:     mixMod += value * 0.5f;   break;
                case DestFmAmt:   fmMod  += value;          break;
                case DestCutoff:  cutoffMod += value;       break; // se interpreta como octavas
                case DestRes:     resMod    += value;       break;
                default: break;
            }
        };
        applyMod(lfo1Dest, lfo1Val);
        applyMod(lfo2Dest, lfo2Val);
        applyMod(modEnvDest, modEnvNext);

        // Glide hacia la nota actual
        const float glidedNote = glidePitchSemis.getNextValue();
        const float pitchOffsetFromBase = (glidedNote - (float)currentMidiNote) + pitchModSemis;

        // Aplicar mods de PW
        const float effPw1 = juce::jlimit(0.05f, 0.95f, pw1 + pwMod1);
        const float effPw2 = juce::jlimit(0.05f, 0.95f, pw2 + pwMod2);

        // Aplicar mix modulado
        const float effMix    = juce::jlimit(0.0f, 1.0f, mixAmount + mixMod);
        const float effFmAmt  = juce::jlimit(0.0f, 1.0f, fmAmount + fmMod);

        // Renderizar osciladores con unison
        float osc1Sum = 0.0f, osc2Sum = 0.0f;
        const int U = unisonVoices;

        for (int u = 0; u < U; ++u)
        {
            osc1[u].setExtraPitchOffset(pitchOffsetFromBase);
            osc2[u].setExtraPitchOffset(pitchOffsetFromBase);
            osc1[u].setPulseWidth(effPw1);
            osc2[u].setPulseWidth(effPw2);

            bool wrappedMaster = false;
            float s2 = 0.0f;

            // En modo FM: OSC2 (modulador) modula la fase de OSC1 (portador)
            // En modo Sync: OSC1 (master) resetea OSC2 (slave) cuando completa ciclo
            if (oscMode == ModeFM)
            {
                bool wrappedMod = false;
                const float mod = osc2[u].renderSample(wrappedMod);
                // FM amount mapeado a una desviacion de fase (radianes)
                const float pmRad = mod * effFmAmt * juce::MathConstants<float>::twoPi;
                const float s1 = osc1[u].renderSampleWithPhaseMod(pmRad, wrappedMaster);
                osc1Sum += s1;
                // En FM solo suena el portador
            }
            else if (oscMode == ModeSync)
            {
                const float s1 = osc1[u].renderSample(wrappedMaster);
                if (wrappedMaster) osc2[u].hardSyncResetFromMaster();
                bool wrappedSlave = false;
                s2 = osc2[u].renderSample(wrappedSlave);
                osc1Sum += s1;
                osc2Sum += s2;
            }
            else
            {
                const float s1 = osc1[u].renderSample(wrappedMaster);
                bool wrappedB = false;
                s2 = osc2[u].renderSample(wrappedB);
                osc1Sum += s1;
                osc2Sum += s2;
            }
        }

        // Normalizar el unison
        if (U > 0)
        {
            const float norm = 1.0f / std::sqrt((float)U);
            osc1Sum *= norm;
            osc2Sum *= norm;
        }

        // Combinar OSC1 + OSC2 segun modo
        float combined = 0.0f;
        switch (oscMode)
        {
            case ModeMix:
                combined = (1.0f - effMix) * osc1Sum + effMix * osc2Sum;
                break;
            case ModeRing:
                combined = osc1Sum * osc2Sum;
                break;
            case ModeSync:
                combined = (1.0f - effMix) * osc1Sum + effMix * osc2Sum;
                break;
            case ModeFM:
                combined = osc1Sum; // solo portador
                break;
        }

        // Sub-oscilador
        bool subWrap = false;
        const float subSample = sub.renderSample(subWrap) * subLevel;
        combined += subSample;

        // === Filtro por voz ===
        // Cutoff efectivo:
        //   - parte del cutoff base
        //   - key tracking: sube/baja segun la nota MIDI tocada (semitonos)
        //   - cutoffMod: las modulaciones (LFO/ModEnv) se interpretan en octavas
        const float keyTrackSemis = ((float)currentMidiNote - 69.0f) * filterKeyTrack;
        const float modOctaves    = cutoffMod * 4.0f; // hasta +/- 4 octavas con depth maximo
        const float effCutoff = juce::jlimit(20.0f, 20000.0f,
                                             filterCutoff * std::pow(2.0f,
                                                                     keyTrackSemis / 12.0f
                                                                     + modOctaves));
        const float effRes = juce::jlimit(0.1f, 12.0f, filterResonance + resMod * 6.0f);
        filter.setParameters(filterType, effCutoff, effRes, filterDrive);
        combined = filter.processSample(combined);

        // Envelope de amplitud + velocity sensitivity + amp mod
        const float envAmp = ampAdsr.getNextSample();
        const float velFactor = juce::jmap(velocitySens, 1.0f, currentVelocity);
        const float ampMix = juce::jlimit(0.0f, 2.0f, 1.0f + ampMod);
        const float gainSample = envAmp * velFactor * ampMix * masterGain * 0.5f;

        const float finalSample = combined * gainSample;

        // Estereo: si hay unison spread y dos canales, vamos a desplazar ligeramente
        // (aproximacion simple: hard-panning del unison no implementado en esta version)
        left[i] += finalSample;
        if (right != nullptr) right[i] += finalSample;
    }

    // Aplicar buffer al output
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);

    if (!ampAdsr.isActive())
        clearCurrentNote();
}
