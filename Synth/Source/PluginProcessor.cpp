/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthAudioProcessor::SynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts (*this, nullptr, "APVTS", createParams())
#endif
{
    synth.addSound(new SynthSound());

    // Polifonia: 8 voces
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new SynthVoice());
}

SynthAudioProcessor::~SynthAudioProcessor()
{
    synth.setCurrentPlaybackSampleRate (getSampleRate());
}

//==============================================================================
const juce::String SynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void SynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	synth.setCurrentPlaybackSampleRate(sampleRate);

	for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }

    effects.prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void SynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Leer todos los parametros una vez por bloque
    auto P = [&](const char* id) { return apvts.getRawParameterValue(id)->load(); };

    // Amp env
    const float aAtk = P("ATTACK"),  aDec = P("DECAY"), aSus = P("SUSTAIN"), aRel = P("RELEASE");
    // Mod env
    const float mAtk = P("MENV_ATTACK"),  mDec = P("MENV_DECAY"),
                mSus = P("MENV_SUSTAIN"), mRel = P("MENV_RELEASE");
    const int   mDest  = (int)P("MENV_DEST");
    const float mDepth = P("MENV_DEPTH");

    // OSC1
    const int   w1 = (int)P("OSC1WAVETYPE");
    const int   o1 = (int)P("OSC1OCTAVE");
    const int   s1 = (int)P("OSC1SEMI");
    const float f1 = P("OSC1FINE");
    const float p1 = P("OSC1PW");
    // OSC2
    const int   w2 = (int)P("OSC2WAVETYPE");
    const int   o2 = (int)P("OSC2OCTAVE");
    const int   s2 = (int)P("OSC2SEMI");
    const float f2 = P("OSC2FINE");
    const float p2 = P("OSC2PW");

    // Sub
    const int   subW = (int)P("SUB_WAVE");
    const float subL = P("SUB_LEVEL");

    // Mix / mode / FM
    const float mix    = P("OSC_MIX");
    const int   mode   = (int)P("OSC_MODE");
    const float fmAmt  = P("FM_AMOUNT");

    // LFOs
    const int   l1w = (int)P("LFO1_WAVE");  const float l1r = P("LFO1_RATE");
    const float l1d = P("LFO1_DEPTH");      const int   l1D = (int)P("LFO1_DEST");

    const int   l2w = (int)P("LFO2_WAVE");  const float l2r = P("LFO2_RATE");
    const float l2d = P("LFO2_DEPTH");      const int   l2D = (int)P("LFO2_DEST");

    // Master
    const float mGain = P("MASTER_GAIN");
    const float vel   = P("VEL_SENS");
    const float glide = P("GLIDE");
    const int   vMode = (int)P("VOICE_MODE");

    // Unison
    const int   uVoices = (int)P("UNISON_VOICES");
    const float uDet    = P("UNISON_DETUNE");
    const float uSpread = P("UNISON_SPREAD");

    // Filter
    const int   fType   = (int)P("FILTER_TYPE");
    const float fCut    = P("FILTER_CUTOFF");
    const float fRes    = P("FILTER_RES");
    const float fDrive  = P("FILTER_DRIVE");
    const float fKT     = P("FILTER_KEYTRACK");

    // FX - Chorus
    const bool  chOn    = P("CHORUS_ON") > 0.5f;
    const float chRate  = P("CHORUS_RATE");
    const float chDepth = P("CHORUS_DEPTH");
    const float chFb    = P("CHORUS_FB");
    const float chMix   = P("CHORUS_MIX");
    // FX - Delay
    const bool  dlOn    = P("DELAY_ON") > 0.5f;
    const float dlTime  = P("DELAY_TIME");
    const float dlFb    = P("DELAY_FB");
    const float dlMix   = P("DELAY_MIX");
    // FX - Reverb
    const bool  rvOn    = P("REVERB_ON") > 0.5f;
    const float rvSize  = P("REVERB_SIZE");
    const float rvDamp  = P("REVERB_DAMP");
    const float rvWidth = P("REVERB_WIDTH");
    const float rvMix   = P("REVERB_MIX");

    effects.setChorusEnabled(chOn);
    effects.setChorusParams(chRate, chDepth, chFb, chMix);
    effects.setDelayEnabled(dlOn);
    effects.setDelayParams(dlTime, dlFb, dlMix);
    effects.setReverbEnabled(rvOn);
    effects.setReverbParams(rvSize, rvDamp, rvWidth, rvMix);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->setAmpAdsr   (aAtk, aDec, aSus, aRel);
            voice->setModAdsr   (mAtk, mDec, mSus, mRel);
            voice->setModEnv    (mDest, mDepth);
            voice->setOscParams (w1, o1, s1, f1, p1, w2, o2, s2, f2, p2);
            voice->setSubParams (subW, subL);
            voice->setMixAndMode(mix, mode, fmAmt);
            voice->setLfo1      (l1w, l1r, l1d, l1D);
            voice->setLfo2      (l2w, l2r, l2d, l2D);
            voice->setMaster    (mGain, vel, glide, vMode);
            voice->setUnison    (uVoices, uDet, uSpread);
            voice->setFilter    (fType, fCut, fRes, fDrive, fKT);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Cadena de efectos globales (post-synth)
    effects.process(buffer);

    // Empuja la senial al buffer del osciloscopio (sin downsample, para que la
    // FFT tenga la resolucion completa). El osciloscopio decima al dibujar.
    scopeBuffer.sampleRate.store(getSampleRate(), std::memory_order_relaxed);
    {
        const int n = buffer.getNumSamples();
        const float* ch = buffer.getReadPointer(0);
        for (int i = 0; i < n; ++i)
            scopeBuffer.push(ch[i]);
    }
}

//==============================================================================
bool SynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SynthAudioProcessor::createEditor()
{
    return new SynthAudioProcessorEditor (*this);
}

//==============================================================================
void SynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Guarda el estado del APVTS como XML serializado en el bloque de memoria del host
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restaura el estado del APVTS desde el bloque de memoria del host
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout SynthAudioProcessor::createParams()
{
    using FRange  = juce::NormalisableRange<float>;
    using PFloat  = juce::AudioParameterFloat;
    using PInt    = juce::AudioParameterInt;
    using PChoice = juce::AudioParameterChoice;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    const juce::StringArray oscWaves   { "Sine","Triangle","Saw","Square","Pulse","Noise" };
    const juce::StringArray lfoWaves   { "Sine","Triangle","Saw","Square","S&H" };
    const juce::StringArray subWaves   { "Sine","Triangle","Saw","Square","Pulse","Noise" };
    const juce::StringArray oscModes   { "Mix","Ring","Sync","FM" };
    const juce::StringArray modDests   { "None","Pitch","Osc1 PW","Osc2 PW","Amp","Mix","FM Amt","Cutoff","Reso" };
    const juce::StringArray filterTypes{ "LP","HP","BP","Notch" };
    const juce::StringArray voiceModes { "Poly","Mono","Legato" };

    // === OSC1 ===
    p.push_back(std::make_unique<PChoice>("OSC1WAVETYPE", "Osc 1 Wave", oscWaves, 0));
    p.push_back(std::make_unique<PInt>   ("OSC1OCTAVE",   "Osc 1 Octave", -4, 4, 0));
    p.push_back(std::make_unique<PInt>   ("OSC1SEMI",     "Osc 1 Semi", -12, 12, 0));
    p.push_back(std::make_unique<PFloat> ("OSC1FINE",     "Osc 1 Fine", FRange{ -100.0f, 100.0f, 0.1f }, 0.0f));
    p.push_back(std::make_unique<PFloat> ("OSC1PW",       "Osc 1 PW",   FRange{ 0.05f, 0.95f, 0.001f }, 0.5f));

    // === OSC2 ===
    p.push_back(std::make_unique<PChoice>("OSC2WAVETYPE", "Osc 2 Wave", oscWaves, 2));
    p.push_back(std::make_unique<PInt>   ("OSC2OCTAVE",   "Osc 2 Octave", -4, 4, 0));
    p.push_back(std::make_unique<PInt>   ("OSC2SEMI",     "Osc 2 Semi", -12, 12, 0));
    p.push_back(std::make_unique<PFloat> ("OSC2FINE",     "Osc 2 Fine", FRange{ -100.0f, 100.0f, 0.1f }, 7.0f));
    p.push_back(std::make_unique<PFloat> ("OSC2PW",       "Osc 2 PW",   FRange{ 0.05f, 0.95f, 0.001f }, 0.5f));

    // === SUB ===
    p.push_back(std::make_unique<PChoice>("SUB_WAVE",  "Sub Wave", subWaves, 0));
    p.push_back(std::make_unique<PFloat> ("SUB_LEVEL", "Sub Level", FRange{ 0.0f, 1.0f, 0.001f }, 0.0f));

    // === MIX / MODE / FM ===
    p.push_back(std::make_unique<PFloat> ("OSC_MIX",   "Osc Mix",  FRange{ 0.0f, 1.0f, 0.001f }, 0.5f));
    p.push_back(std::make_unique<PChoice>("OSC_MODE",  "Osc Mode", oscModes, 0));
    p.push_back(std::make_unique<PFloat> ("FM_AMOUNT", "FM Amount", FRange{ 0.0f, 1.0f, 0.001f }, 0.0f));

    // === AMP ADSR ===
    p.push_back(std::make_unique<PFloat>("ATTACK",  "Attack",  FRange{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.01f));
    p.push_back(std::make_unique<PFloat>("DECAY",   "Decay",   FRange{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.1f));
    p.push_back(std::make_unique<PFloat>("SUSTAIN", "Sustain", FRange{ 0.0f, 1.0f, 0.001f }, 1.0f));
    p.push_back(std::make_unique<PFloat>("RELEASE", "Release", FRange{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.4f));

    // === MOD ENV ===
    p.push_back(std::make_unique<PFloat> ("MENV_ATTACK",  "Mod Attack",  FRange{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.01f));
    p.push_back(std::make_unique<PFloat> ("MENV_DECAY",   "Mod Decay",   FRange{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.2f));
    p.push_back(std::make_unique<PFloat> ("MENV_SUSTAIN", "Mod Sustain", FRange{ 0.0f, 1.0f, 0.001f }, 0.0f));
    p.push_back(std::make_unique<PFloat> ("MENV_RELEASE", "Mod Release", FRange{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.3f));
    p.push_back(std::make_unique<PFloat> ("MENV_DEPTH",   "Mod Depth",   FRange{ -1.0f, 1.0f, 0.001f }, 0.0f));
    p.push_back(std::make_unique<PChoice>("MENV_DEST",    "Mod Dest", modDests, 0));

    // === LFO 1 ===
    p.push_back(std::make_unique<PChoice>("LFO1_WAVE",  "LFO1 Wave", lfoWaves, 0));
    p.push_back(std::make_unique<PFloat> ("LFO1_RATE",  "LFO1 Rate", FRange{ 0.01f, 30.0f, 0.01f, 0.5f }, 1.0f));
    p.push_back(std::make_unique<PFloat> ("LFO1_DEPTH", "LFO1 Depth", FRange{ 0.0f, 1.0f, 0.001f }, 0.0f));
    p.push_back(std::make_unique<PChoice>("LFO1_DEST",  "LFO1 Dest", modDests, 0));

    // === LFO 2 ===
    p.push_back(std::make_unique<PChoice>("LFO2_WAVE",  "LFO2 Wave", lfoWaves, 0));
    p.push_back(std::make_unique<PFloat> ("LFO2_RATE",  "LFO2 Rate", FRange{ 0.01f, 30.0f, 0.01f, 0.5f }, 1.0f));
    p.push_back(std::make_unique<PFloat> ("LFO2_DEPTH", "LFO2 Depth", FRange{ 0.0f, 1.0f, 0.001f }, 0.0f));
    p.push_back(std::make_unique<PChoice>("LFO2_DEST",  "LFO2 Dest", modDests, 0));

    // === MASTER ===
    p.push_back(std::make_unique<PFloat> ("MASTER_GAIN", "Master Gain", FRange{ 0.0f, 1.0f, 0.001f }, 0.7f));
    p.push_back(std::make_unique<PFloat> ("VEL_SENS",    "Velocity Sens", FRange{ 0.0f, 1.0f, 0.001f }, 0.5f));
    p.push_back(std::make_unique<PFloat> ("GLIDE",       "Glide", FRange{ 0.0f, 2.0f, 0.001f, 0.5f }, 0.0f));
    p.push_back(std::make_unique<PChoice>("VOICE_MODE",  "Voice Mode", voiceModes, 0));

    // === UNISON ===
    p.push_back(std::make_unique<PInt>   ("UNISON_VOICES", "Unison Voices", 1, 7, 1));
    p.push_back(std::make_unique<PFloat> ("UNISON_DETUNE", "Unison Detune", FRange{ 0.0f, 50.0f, 0.1f }, 10.0f));
    p.push_back(std::make_unique<PFloat> ("UNISON_SPREAD", "Unison Spread", FRange{ 0.0f, 1.0f, 0.001f }, 0.5f));

    // === FILTER ===
    // Cutoff: rango logaritmico de 20 Hz a 20 kHz (skew para que el knob sea musical)
    p.push_back(std::make_unique<PChoice>("FILTER_TYPE",  "Filter Type", filterTypes, 0));
    p.push_back(std::make_unique<PFloat> ("FILTER_CUTOFF", "Filter Cutoff",
        FRange{ 20.0f, 20000.0f, 1.0f, 0.25f }, 12000.0f));
    p.push_back(std::make_unique<PFloat> ("FILTER_RES",   "Filter Resonance",
        FRange{ 0.1f, 12.0f, 0.01f, 0.5f }, 0.7f));
    p.push_back(std::make_unique<PFloat> ("FILTER_DRIVE", "Filter Drive",
        FRange{ 1.0f, 12.0f, 0.01f, 0.5f }, 1.0f));
    p.push_back(std::make_unique<PFloat> ("FILTER_KEYTRACK", "Filter Key Track",
        FRange{ 0.0f, 1.0f, 0.001f }, 0.0f));

    // === FX - CHORUS ===
    p.push_back(std::make_unique<juce::AudioParameterBool>("CHORUS_ON", "Chorus On", false));
    p.push_back(std::make_unique<PFloat>("CHORUS_RATE",  "Chorus Rate",  FRange{ 0.1f, 8.0f, 0.01f, 0.5f }, 1.2f));
    p.push_back(std::make_unique<PFloat>("CHORUS_DEPTH", "Chorus Depth", FRange{ 0.0f, 1.0f, 0.001f }, 0.4f));
    p.push_back(std::make_unique<PFloat>("CHORUS_FB",    "Chorus Feedback", FRange{ 0.0f, 0.9f, 0.001f }, 0.0f));
    p.push_back(std::make_unique<PFloat>("CHORUS_MIX",   "Chorus Mix",   FRange{ 0.0f, 1.0f, 0.001f }, 0.3f));

    // === FX - DELAY ===
    p.push_back(std::make_unique<juce::AudioParameterBool>("DELAY_ON", "Delay On", false));
    p.push_back(std::make_unique<PFloat>("DELAY_TIME", "Delay Time (ms)", FRange{ 1.0f, 1500.0f, 1.0f, 0.5f }, 300.0f));
    p.push_back(std::make_unique<PFloat>("DELAY_FB",   "Delay Feedback",  FRange{ 0.0f, 0.95f, 0.001f }, 0.35f));
    p.push_back(std::make_unique<PFloat>("DELAY_MIX",  "Delay Mix",       FRange{ 0.0f, 1.0f, 0.001f }, 0.3f));

    // === FX - REVERB ===
    p.push_back(std::make_unique<juce::AudioParameterBool>("REVERB_ON", "Reverb On", false));
    p.push_back(std::make_unique<PFloat>("REVERB_SIZE",  "Reverb Size",  FRange{ 0.0f, 1.0f, 0.001f }, 0.5f));
    p.push_back(std::make_unique<PFloat>("REVERB_DAMP",  "Reverb Damp",  FRange{ 0.0f, 1.0f, 0.001f }, 0.5f));
    p.push_back(std::make_unique<PFloat>("REVERB_WIDTH", "Reverb Width", FRange{ 0.0f, 1.0f, 0.001f }, 1.0f));
    p.push_back(std::make_unique<PFloat>("REVERB_MIX",   "Reverb Mix",   FRange{ 0.0f, 1.0f, 0.001f }, 0.3f));

    return { p.begin(), p.end() };
}

//Value Tree
