/*
  ==============================================================================

    Grain.h
    Created: 16 Nov 2024 10:05:50pm
    Author:  David Matthew Welch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Grain
{
public:
    /**
     * Constructor for the Grain class.
     *
     * @param sourceBuffer      The audio buffer from which the grain is extracted.
     * @param startSample       The starting sample index in the source buffer.
     * @param grainSize         The size of the grain in samples.
     * @param pitchShiftFactor  The factor by which to shift the pitch (e.g., 1.0 = no shift).
     * @param sampleRate        The current sample rate of the audio processor.
     */
    Grain(const juce::AudioBuffer<float>& sourceBuffer, int startSample,
          int grainSize, float pitchShiftFactor, double sampleRate);

    /**
     * Processes the grain and adds it to the output buffer.
     *
     * @param outputBuffer         The buffer to which the grain's audio will be added.
     * @param startSampleInOutput  The starting sample index in the output buffer.
     */
    void processGrain(juce::AudioBuffer<float>& outputBuffer, int startSampleInOutput);

    /**
     * Checks if the grain has finished processing.
     *
     * @return True if the grain has finished processing, false otherwise.
     */
    bool isFinished() const;

private:
    juce::AudioBuffer<float> grainAudioData;  // The audio data of the grain
    int currentPosition = 0;                  // The current position within the grain
    int size = 0;                             // The size of the grain in samples
    float pitchShiftFactor = 1.0f;            // The pitch shift factor
    std::vector<float> window;                // The windowing function applied to the grain
    double currentSampleRate = 44100.0;       // The current sample rate

    /**
     * Applies a Hanning window to the grain's audio data.
     */
    void applyWindow();

    /**
     * Applies pitch shifting to the grain's audio data using LagrangeInterpolator.
     */
    void applyPitchShift();
};
