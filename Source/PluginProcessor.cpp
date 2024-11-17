/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Hw5AudioProcessor::Hw5AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
            apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}
juce::AudioProcessorValueTreeState::ParameterLayout Hw5AudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>("GRAIN_SIZE", "Grain Size", 1, 2048, 512));
    params.push_back(std::make_unique<juce::AudioParameterInt>("GRAIN_OVERLAP", "Grain Overlap", 0, 2048, 256));
    params.push_back(std::make_unique<juce::AudioParameterInt>("GRAIN_SPACING", "Grain Spacing", 0, 2048, 0));

    return { params.begin(), params.end() };
}


Hw5AudioProcessor::~Hw5AudioProcessor()
{
}

//==============================================================================
const juce::String Hw5AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Hw5AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Hw5AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Hw5AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Hw5AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Hw5AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Hw5AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Hw5AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Hw5AudioProcessor::getProgramName (int index)
{
    return {};
}

void Hw5AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Hw5AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    granSynth.prepareToPlay(sampleRate, samplesPerBlock);
    // Set initial grain parameters
    updateGrainParameters();

}

void Hw5AudioProcessor::releaseResources()
{
    granSynth.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Hw5AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Hw5AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
//    juce::ScopedNoDenormals noDenormals;
    
    // Update grain parameters in case they have changed
    updateGrainParameters();

    granSynth.processBlock(buffer, midiMessages);
    granSynth.processBlock(buffer, midiMessages);
}

void Hw5AudioProcessor::updateGrainParameters()
{
    int grainSize = apvts.getRawParameterValue("GRAIN_SIZE")->load();
    int grainOverlap = apvts.getRawParameterValue("GRAIN_OVERLAP")->load();
    int grainSpacing = apvts.getRawParameterValue("GRAIN_SPACING")->load();

    granSynth.setGrainParameters(grainSize, grainOverlap, grainSpacing);
}

void Hw5AudioProcessor::loadAudioFile(const juce::File& audioFile)
{
    granSynth.loadAudioFile(audioFile);
}

//==============================================================================
bool Hw5AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Hw5AudioProcessor::createEditor()
{
    return new Hw5AudioProcessorEditor (*this);
}

//==============================================================================
void Hw5AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Hw5AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Hw5AudioProcessor();
}
