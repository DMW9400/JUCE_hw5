/*
  ==============================================================================

    GranSynth.cpp
    Created: 16 Nov 2024 8:08:27pm
    Author:  David Matthew Welch

  ==============================================================================
*/

#include "GranSynth.h"

GranSynth::GranSynth()
{
    formatManager.registerBasicFormats();
}

GranSynth::~GranSynth()
{
    releaseResources();
}

void GranSynth::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentSamplesPerBlock = samplesPerBlock;
}

void GranSynth::releaseResources()
{
    grains.clear();
}

void GranSynth::setGrainParameters(int size, int overlap, int spacing)
{
    grainSize = size;
    grainOverlap = overlap;
    grainSpacing = spacing;
}

void GranSynth::loadAudioFile(const juce::File& audioFile)
{
    if (!audioFile.existsAsFile())
    {
        DBG("Selected file does not exist.");
        return;
    }

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(audioFile));

    if (reader != nullptr)
    {
        sourceBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&sourceBuffer, 0, (int)reader->lengthInSamples, 0, true, true);

        DBG("Audio file loaded successfully.");
    }
    else
    {
        DBG("Failed to create AudioFormatReader for the selected file.");
    }
}

void GranSynth::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    handleMidi(midiMessages);

    int numSamples = buffer.getNumSamples();
    sampleCounter += numSamples;

    // Check if it's time to create a new grain
    int grainInterval = grainSize - grainOverlap + grainSpacing;
    if (sampleCounter >= grainInterval)
    {
        sampleCounter = 0;

        // Remove finished grains
        grains.erase(std::remove_if(grains.begin(), grains.end(),
            [](const std::unique_ptr<Grain>& grain) { return grain->isFinished(); }),
            grains.end());

        // Create new grains if notes are active
        // For simplicity, we create grains continuously
        if (sourceBuffer.getNumSamples() > 0)
        {
            int startSample = juce::Random::getSystemRandom().nextInt(sourceBuffer.getNumSamples() - grainSize);
            float pitchShiftFactor = 1.0f; // Adjust as needed

            grains.push_back(std::make_unique<Grain>(sourceBuffer, startSample, grainSize, pitchShiftFactor, currentSampleRate));
        }
    }

    // Process and mix all grains into the output buffer
    for (auto& grain : grains)
    {
        grain->processGrain(buffer, 0); // Adjust startSampleInOutput as needed
    }
}

void GranSynth::handleMidi(const juce::MidiBuffer& midiMessages)
{
    for (const auto metadata : midiMessages)
    {
        const auto& message = metadata.getMessage();
        const int samplePosition = metadata.samplePosition;

        if (message.isNoteOn())
        {
            int midiNoteNumber = message.getNoteNumber();
            float pitchShiftFactor = midiNoteToPitchShift(midiNoteNumber);

            int startSample = juce::Random::getSystemRandom().nextInt(sourceBuffer.getNumSamples() - grainSize);
            grains.push_back(std::make_unique<Grain>(sourceBuffer, startSample, grainSize, pitchShiftFactor, currentSampleRate));
        }
        else if (message.isNoteOff())
        {
            // Handle note-off events if necessary
            grains.clear();
        }
        else if (message.isAllNotesOff())
        {
            grains.clear();
        }
    }
}

float GranSynth::midiNoteToPitchShift(int midiNoteNumber)
{
    // Convert MIDI note number to frequency ratio
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    float baseFrequency = 440.0f; // Reference frequency (A4)
    return frequency / baseFrequency;
}
