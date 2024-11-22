/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Hw5AudioProcessorEditor  : public juce::AudioProcessorEditor,
                                 public juce::FileDragAndDropTarget
{
public:
    /**
     * Constructor.
     *
     * @param processor  A reference to the audio processor associated with this editor.
     */
    Hw5AudioProcessorEditor (Hw5AudioProcessor& processor);

    /**
     * Destructor.
     */
    ~Hw5AudioProcessorEditor() override;

    /**
     * Paints the UI components.
     *
     * @param g  The graphics context to use for painting.
     */
    void paint (juce::Graphics&) override;

    /**
     * Resizes and lays out the UI components.
     */
    void resized() override;

    // FileDragAndDropTarget methods

    /**
     * Determines if the editor is interested in the files being dragged over it.
     *
     * @param files  The list of files being dragged.
     * @return       True if the editor can accept the files, false otherwise.
     */
    bool isInterestedInFileDrag (const juce::StringArray& files) override;

    /**
     * Handles files being dropped onto the editor.
     *
     * @param files  The list of files dropped.
     * @param x      The x-coordinate of the drop location.
     * @param y      The y-coordinate of the drop location.
     */
    void filesDropped (const juce::StringArray& files, int x, int y) override;

private:
    Hw5AudioProcessor& audioProcessor; // Reference to the audio processor

    // UI Components
    juce::Slider grainSizeSlider;
    juce::Slider grainOverlapSlider;
    juce::Slider grainSpacingSlider;

    juce::Label grainSizeLabel;
    juce::Label grainOverlapLabel;
    juce::Label grainSpacingLabel;

    juce::TextButton loadFileButton;

    // Attachment classes for parameter control
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> grainSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> grainOverlapAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> grainSpacingAttachment;

    /**
     * Opens a file chooser dialog to load an audio file.
     */
    void loadAudioFile();
    
    std::unique_ptr<juce::FileChooser> fileChooser;


    /**
     * Callback for when the load file button is clicked.
     */
    void loadFileButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Hw5AudioProcessorEditor)
};
