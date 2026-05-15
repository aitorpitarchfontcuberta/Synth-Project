/*
  ==============================================================================

    FactoryPresets.cpp

  ==============================================================================
*/

#include "FactoryPresets.h"

namespace FactoryPresets {

// Helper: rellena defaults razonables que despues cada preset puede sobreescribir.
// Asi cada preset solo necesita declarar los parametros que difieren del "Init".
static std::map<juce::String, float> withDefaults(std::initializer_list<std::pair<juce::String, float>> overrides)
{
    std::map<juce::String, float> m;
    // OSC1
    m["OSC1WAVETYPE"] = 2.0f;   // Saw
    m["OSC1OCTAVE"]   = 0.0f;
    m["OSC1SEMI"]     = 0.0f;
    m["OSC1FINE"]     = 0.0f;
    m["OSC1PW"]       = 0.5f;
    // OSC2
    m["OSC2WAVETYPE"] = 2.0f;
    m["OSC2OCTAVE"]   = 0.0f;
    m["OSC2SEMI"]     = 0.0f;
    m["OSC2FINE"]     = 0.0f;
    m["OSC2PW"]       = 0.5f;
    // Sub
    m["SUB_WAVE"]  = 0.0f;
    m["SUB_LEVEL"] = 0.0f;
    // Mix / mode / FM
    m["OSC_MIX"]   = 0.5f;
    m["OSC_MODE"]  = 0.0f;  // Mix
    m["FM_AMOUNT"] = 0.0f;
    // Amp env
    m["ATTACK"]  = 0.01f;
    m["DECAY"]   = 0.2f;
    m["SUSTAIN"] = 1.0f;
    m["RELEASE"] = 0.4f;
    // Mod env
    m["MENV_ATTACK"]  = 0.01f;
    m["MENV_DECAY"]   = 0.3f;
    m["MENV_SUSTAIN"] = 0.0f;
    m["MENV_RELEASE"] = 0.3f;
    m["MENV_DEPTH"]   = 0.0f;
    m["MENV_DEST"]    = 0.0f; // None
    // LFOs
    m["LFO1_WAVE"]  = 0.0f;
    m["LFO1_RATE"]  = 1.0f;
    m["LFO1_DEPTH"] = 0.0f;
    m["LFO1_DEST"]  = 0.0f;
    m["LFO2_WAVE"]  = 0.0f;
    m["LFO2_RATE"]  = 1.0f;
    m["LFO2_DEPTH"] = 0.0f;
    m["LFO2_DEST"]  = 0.0f;
    // Master
    m["MASTER_GAIN"] = 0.7f;
    m["VEL_SENS"]    = 0.5f;
    m["GLIDE"]       = 0.0f;
    m["VOICE_MODE"]  = 0.0f; // Poly
    // Unison
    m["UNISON_VOICES"] = 1.0f;
    m["UNISON_DETUNE"] = 10.0f;
    m["UNISON_SPREAD"] = 0.5f;
    // Filter (transparente)
    m["FILTER_TYPE"]     = 0.0f; // LP
    m["FILTER_CUTOFF"]   = 12000.0f;
    m["FILTER_RES"]      = 0.7f;
    m["FILTER_DRIVE"]    = 1.0f;
    m["FILTER_KEYTRACK"] = 0.0f;

    for (auto& kv : overrides)
        m[kv.first] = kv.second;
    return m;
}

const std::vector<FactoryPreset>& getAll()
{
    static const std::vector<FactoryPreset> all = {
        // ---- Basic ----
        {
            "Init", "Basic",
            withDefaults({})
        },

        // ---- Bass ----
        {
            "Sub Bass", "Bass",
            withDefaults({
                {"OSC1WAVETYPE", 0.0f},   // Sine
                {"OSC1OCTAVE",  -1.0f},
                {"OSC2WAVETYPE", 0.0f},
                {"OSC2OCTAVE",  -2.0f},
                {"OSC2FINE",    -5.0f},
                {"SUB_WAVE",     0.0f},
                {"SUB_LEVEL",    0.7f},
                {"OSC_MIX",      0.3f},
                {"ATTACK",       0.002f},
                {"DECAY",        0.3f},
                {"SUSTAIN",      0.7f},
                {"RELEASE",      0.15f},
                {"FILTER_CUTOFF",1500.0f},
                {"FILTER_RES",   1.2f},
                {"VOICE_MODE",   1.0f},   // Mono
            })
        },
        {
            "Reese Bass", "Bass",
            withDefaults({
                {"OSC1WAVETYPE", 2.0f},   // Saw
                {"OSC1OCTAVE",  -1.0f},
                {"OSC2WAVETYPE", 2.0f},
                {"OSC2OCTAVE",  -1.0f},
                {"OSC2FINE",     14.0f},
                {"OSC_MIX",      0.5f},
                {"UNISON_VOICES",5.0f},
                {"UNISON_DETUNE",18.0f},
                {"ATTACK",       0.005f},
                {"DECAY",        0.4f},
                {"SUSTAIN",      0.85f},
                {"RELEASE",      0.3f},
                {"FILTER_CUTOFF",2200.0f},
                {"FILTER_RES",   1.8f},
                {"FILTER_DRIVE", 2.5f},
            })
        },
        {
            "Acid Bass", "Bass",
            withDefaults({
                {"OSC1WAVETYPE", 2.0f},
                {"OSC1OCTAVE",  -1.0f},
                {"OSC2WAVETYPE", 3.0f},   // Square
                {"OSC2OCTAVE",  -1.0f},
                {"OSC_MIX",      0.2f},
                {"ATTACK",       0.002f},
                {"DECAY",        0.25f},
                {"SUSTAIN",      0.4f},
                {"RELEASE",      0.1f},
                {"MENV_ATTACK",  0.001f},
                {"MENV_DECAY",   0.25f},
                {"MENV_SUSTAIN", 0.0f},
                {"MENV_RELEASE", 0.1f},
                {"MENV_DEPTH",   0.55f},
                {"MENV_DEST",    7.0f},   // Cutoff
                {"FILTER_CUTOFF",450.0f},
                {"FILTER_RES",   7.5f},
                {"FILTER_DRIVE", 3.0f},
                {"FILTER_KEYTRACK", 0.5f},
                {"VOICE_MODE",   2.0f},   // Legato
                {"GLIDE",        0.06f},
            })
        },

        // ---- Lead ----
        {
            "Classic Saw Lead", "Lead",
            withDefaults({
                {"OSC1WAVETYPE", 2.0f},
                {"OSC2WAVETYPE", 2.0f},
                {"OSC2FINE",     8.0f},
                {"OSC_MIX",      0.5f},
                {"ATTACK",       0.005f},
                {"DECAY",        0.2f},
                {"SUSTAIN",      0.85f},
                {"RELEASE",      0.25f},
                {"LFO1_WAVE",    0.0f},   // Sine
                {"LFO1_RATE",    5.5f},
                {"LFO1_DEPTH",   0.05f},
                {"LFO1_DEST",    1.0f},   // Pitch
                {"FILTER_CUTOFF",6500.0f},
                {"VOICE_MODE",   1.0f},   // Mono
            })
        },
        {
            "PWM Lead", "Lead",
            withDefaults({
                {"OSC1WAVETYPE", 4.0f},   // Pulse
                {"OSC1PW",       0.5f},
                {"OSC2WAVETYPE", 4.0f},
                {"OSC2PW",       0.5f},
                {"OSC2FINE",    -7.0f},
                {"OSC_MIX",      0.5f},
                {"ATTACK",       0.01f},
                {"DECAY",        0.25f},
                {"SUSTAIN",      0.8f},
                {"RELEASE",      0.4f},
                {"LFO1_WAVE",    1.0f},   // Triangle
                {"LFO1_RATE",    0.35f},
                {"LFO1_DEPTH",   0.6f},
                {"LFO1_DEST",    2.0f},   // Osc1 PW
                {"LFO2_WAVE",    1.0f},
                {"LFO2_RATE",    0.42f},
                {"LFO2_DEPTH",   0.6f},
                {"LFO2_DEST",    3.0f},   // Osc2 PW
                {"FILTER_CUTOFF",4500.0f},
            })
        },
        {
            "Sync Lead", "Lead",
            withDefaults({
                {"OSC1WAVETYPE", 2.0f},
                {"OSC2WAVETYPE", 2.0f},
                {"OSC2SEMI",     5.0f},
                {"OSC_MODE",     2.0f},   // Sync
                {"OSC_MIX",      0.7f},
                {"ATTACK",       0.005f},
                {"DECAY",        0.3f},
                {"SUSTAIN",      0.85f},
                {"RELEASE",      0.25f},
                {"MENV_DECAY",   0.6f},
                {"MENV_SUSTAIN", 0.4f},
                {"MENV_DEPTH",   0.4f},
                {"MENV_DEST",    7.0f},   // Cutoff
                {"FILTER_CUTOFF",1800.0f},
                {"FILTER_RES",   2.0f},
                {"VOICE_MODE",   1.0f},
            })
        },

        // ---- Pad ----
        {
            "Lush Pad", "Pad",
            withDefaults({
                {"OSC1WAVETYPE", 2.0f},
                {"OSC2WAVETYPE", 2.0f},
                {"OSC2FINE",     9.0f},
                {"OSC_MIX",      0.5f},
                {"SUB_LEVEL",    0.2f},
                {"UNISON_VOICES",7.0f},
                {"UNISON_DETUNE",22.0f},
                {"UNISON_SPREAD",1.0f},
                {"ATTACK",       1.4f},
                {"DECAY",        1.0f},
                {"SUSTAIN",      0.85f},
                {"RELEASE",      2.5f},
                {"LFO1_WAVE",    0.0f},
                {"LFO1_RATE",    0.3f},
                {"LFO1_DEPTH",   0.07f},
                {"LFO1_DEST",    1.0f},   // Pitch
                {"FILTER_CUTOFF",4500.0f},
            })
        },
        {
            "Glass Pad", "Pad",
            withDefaults({
                {"OSC1WAVETYPE", 0.0f},
                {"OSC2WAVETYPE", 0.0f},
                {"OSC2SEMI",     7.0f},
                {"OSC_MODE",     3.0f},   // FM
                {"FM_AMOUNT",    0.35f},
                {"ATTACK",       0.8f},
                {"DECAY",        1.2f},
                {"SUSTAIN",      0.7f},
                {"RELEASE",      2.0f},
                {"MENV_ATTACK",  0.5f},
                {"MENV_DECAY",   2.0f},
                {"MENV_SUSTAIN", 0.0f},
                {"MENV_DEPTH",   0.4f},
                {"MENV_DEST",    6.0f},   // FM Amt
                {"FILTER_CUTOFF",7500.0f},
            })
        },

        // ---- Pluck / Bell ----
        {
            "Soft Pluck", "Pluck",
            withDefaults({
                {"OSC1WAVETYPE", 2.0f},
                {"OSC2WAVETYPE", 0.0f},
                {"OSC_MIX",      0.4f},
                {"ATTACK",       0.001f},
                {"DECAY",        0.4f},
                {"SUSTAIN",      0.0f},
                {"RELEASE",      0.2f},
                {"MENV_ATTACK",  0.001f},
                {"MENV_DECAY",   0.35f},
                {"MENV_SUSTAIN", 0.0f},
                {"MENV_DEPTH",   0.65f},
                {"MENV_DEST",    7.0f},   // Cutoff
                {"FILTER_CUTOFF",800.0f},
                {"FILTER_RES",   1.5f},
            })
        },
        {
            "FM Bell", "Pluck",
            withDefaults({
                {"OSC1WAVETYPE", 0.0f},
                {"OSC2WAVETYPE", 0.0f},
                {"OSC2OCTAVE",   1.0f},
                {"OSC2SEMI",     7.0f},
                {"OSC_MODE",     3.0f},   // FM
                {"FM_AMOUNT",    0.0f},
                {"ATTACK",       0.001f},
                {"DECAY",        1.8f},
                {"SUSTAIN",      0.0f},
                {"RELEASE",      0.5f},
                {"MENV_ATTACK",  0.001f},
                {"MENV_DECAY",   1.6f},
                {"MENV_SUSTAIN", 0.0f},
                {"MENV_DEPTH",   0.9f},
                {"MENV_DEST",    6.0f},   // FM Amt
                {"FILTER_CUTOFF",18000.0f},
            })
        },

        // ---- Effect ----
        {
            "Auto-Wah", "Effect",
            withDefaults({
                {"OSC1WAVETYPE", 2.0f},
                {"OSC2WAVETYPE", 2.0f},
                {"OSC2FINE",     5.0f},
                {"OSC_MIX",      0.5f},
                {"ATTACK",       0.01f},
                {"DECAY",        0.2f},
                {"SUSTAIN",      0.85f},
                {"RELEASE",      0.3f},
                {"LFO1_WAVE",    0.0f},
                {"LFO1_RATE",    2.4f},
                {"LFO1_DEPTH",   0.6f},
                {"LFO1_DEST",    7.0f},   // Cutoff
                {"FILTER_CUTOFF",900.0f},
                {"FILTER_RES",   3.5f},
            })
        },
        {
            "Drone", "Effect",
            withDefaults({
                {"OSC1WAVETYPE", 2.0f},
                {"OSC2WAVETYPE", 2.0f},
                {"OSC2FINE",     15.0f},
                {"SUB_LEVEL",    0.4f},
                {"UNISON_VOICES",5.0f},
                {"UNISON_DETUNE",30.0f},
                {"UNISON_SPREAD",1.0f},
                {"ATTACK",       2.5f},
                {"DECAY",        2.0f},
                {"SUSTAIN",      0.9f},
                {"RELEASE",      3.5f},
                {"LFO1_WAVE",    0.0f},
                {"LFO1_RATE",    0.1f},
                {"LFO1_DEPTH",   0.4f},
                {"LFO1_DEST",    7.0f},
                {"LFO2_WAVE",    0.0f},
                {"LFO2_RATE",    0.07f},
                {"LFO2_DEPTH",   0.08f},
                {"LFO2_DEST",    1.0f},
                {"FILTER_CUTOFF",2500.0f},
                {"FILTER_RES",   1.5f},
            })
        },
    };
    return all;
}

void apply(const FactoryPreset& preset, juce::AudioProcessorValueTreeState& apvts)
{
    for (const auto& [id, value] : preset.params)
    {
        if (auto* p = apvts.getParameter(id))
        {
            const auto range = p->getNormalisableRange();
            const float v = juce::jlimit(range.start, range.end, value);
            p->setValueNotifyingHost(range.convertTo0to1(v));
        }
    }
}

} // namespace FactoryPresets
