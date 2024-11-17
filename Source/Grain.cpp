/*
  ==============================================================================

    Grain.cpp
    Created: 16 Nov 2024 10:05:50pm
    Author:  David Matthew Welch

  ==============================================================================
*/

#include "Grain.h"
#include <JuceHeader.h>

#include <cmath> // For std::cos

Grain::Grain(const juce::AudioBuffer<float>& sourceBuffer, int startSample,
             int grainSize, float pitchShiftFactor, double sampleRate)
    : size(grainSize), pitchShiftFactor(pitchShiftFactor), currentSampleRate(sampleRate)
{
    int numChannels = sourceBuffer.getNumChannels();
    grainAudioData.setSize(numChannels, size);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        if (startSample + size <= sourceBuffer.getNumSamples())
        {
            grainAudioData.copyFrom(channel, 0, sourceBuffer, channel, startSample, size);
        }
        else
        {
            int remainingSamples = sourceBuffer.getNumSamples() - startSample;
            grainAudioData.copyFrom(channel, 0, sourceBuffer, channel, startSample, remainingSamples);
            grainAudioData.copyFrom(channel, remainingSamples, sourceBuffer, channel, 0, size - remainingSamples);
        }
    }

    // Create and apply window
    window.resize(size);
    for (int i = 0; i < size; ++i)
    {
        window[i] = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / (size - 1)));
    }
    applyWindow();

    // Apply pitch shifting
    applyPitchShift();
}

void Grain::applyWindow()
{
    int numChannels = grainAudioData.getNumChannels();
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* data = grainAudioData.getWritePointer(channel);
        for (int i = 0; i < size; ++i)
        {
            data[i] *= window[i];
        }
    }
}

void Grain::applyPitchShift()
{
    if (pitchShiftFactor == 1.0f)
        return;

    int numChannels = grainAudioData.getNumChannels();
    int newSize = static_cast<int>(size / pitchShiftFactor);

    juce::AudioBuffer<float> resampledData;
    resampledData.setSize(numChannels, newSize);
    resampledData.clear();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        juce::LagrangeInterpolator interpolator;
        interpolator.reset();

        const float* srcData = grainAudioData.getReadPointer(channel);
        float* destData = resampledData.getWritePointer(channel);

        double speedRatio = pitchShiftFactor;
        int numOutputSamples = newSize;

        int samplesConsumed = interpolator.process(
            speedRatio,      // double speedRatio
            srcData,         // const float* inputSamples
            destData,        // float* outputSamples
            numOutputSamples // int numOutputSamples
        );

        // Optionally, you can check if samplesConsumed matches expected values
    }

    grainAudioData = resampledData;
    size = newSize;
}

void Grain::processGrain(juce::AudioBuffer<float>& outputBuffer, int startSampleInOutput)
{
    int numChannels = grainAudioData.getNumChannels();
    int outputChannels = outputBuffer.getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* grainData = grainAudioData.getReadPointer(channel);
        float* outputData = outputBuffer.getWritePointer(channel % outputChannels);

        for (int i = 0; i < size; ++i)
        {
            if (startSampleInOutput + i < outputBuffer.getNumSamples())
            {
                outputData[startSampleInOutput + i] += grainData[i];
            }
        }
    }

    currentPosition += size;
}

bool Grain::isFinished() const
{
    return currentPosition >= size;
}
