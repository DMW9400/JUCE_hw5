/*
  ==============================================================================

    GranSynth.h
    Created: 16 Nov 2024 8:08:27pm
    Author:  David Matthew Welch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Grain.h"

class GranSynth
{
public:
    /**
     * Constructor for the GranSynth class.
     */
    GranSynth();

    /**
     * Destructor for the GranSynth class.
     */
    ~GranSynth();

    /**
     * Prepares the synthesizer for playback.
     *
     * @param sampleRate       The current sample rate.
     * @param samplesPerBlock  The maximum number of samples that will be processed in one block.
     */
    void prepareToPlay(double sampleRate, int samplesPerBlock);

    /**
     * Releases any resources used by the synthesizer.
     */
    void releaseResources();

    /**
     * Processes a block of audio and MIDI data.
     *
     * @param buffer         The audio buffer to process.
     * @param midiMessages   The MIDI messages to process.
     */
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

    /**
     * Sets the grain parameters for the synthesizer.
     *
     * @param size     The size of each grain in samples.
     * @param overlap  The overlap between grains in samples.
     * @param spacing  The spacing between grains in samples.
     */
    void setGrainParameters(int size, int overlap, int spacing);

    /**
     * Loads an audio file into the synthesizer.
     *
     * @param audioFile  The audio file to load.
     */
    void loadAudioFile(const juce::File& audioFile);

private:
    std::vector<std::unique_ptr<Grain>> grains; // Vector to manage active grains
    juce::AudioBuffer<float> sourceBuffer;      // Buffer holding the loaded audio file
    juce::AudioFormatManager formatManager;     // Manages audio formats for file loading

    int grainSize = 512;        // Grain size in samples
    int grainOverlap = 256;     // Grain overlap in samples
    int grainSpacing = 0;       // Grain spacing in samples

    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;
    int sampleCounter = 0;

    /**
     * Handles incoming MIDI messages.
     *
     * @param midiMessages  The MIDI messages to handle.
     */
    void handleMidi(const juce::MidiBuffer& midiMessages);

    /**
     * Converts a MIDI note number to a pitch shift factor.
     *
     * @param midiNoteNumber  The MIDI note number to convert.
     * @return                The pitch shift factor.
     */
    float midiNoteToPitchShift(int midiNoteNumber);
};
