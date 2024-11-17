/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
#include "PluginEditor.h"
#include "PluginProcessor.h"

Hw5AudioProcessorEditor::Hw5AudioProcessorEditor (Hw5AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set the editor's size
    setSize (400, 300);

    // Initialize sliders
    grainSizeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    grainSizeSlider.setRange(1, 2048, 1);
    grainSizeSlider.setValue(512);
    grainSizeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addAndMakeVisible(&grainSizeSlider);

    grainOverlapSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    grainOverlapSlider.setRange(0, 2048, 1);
    grainOverlapSlider.setValue(256);
    grainOverlapSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addAndMakeVisible(&grainOverlapSlider);

    grainSpacingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    grainSpacingSlider.setRange(0, 2048, 1);
    grainSpacingSlider.setValue(0);
    grainSpacingSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addAndMakeVisible(&grainSpacingSlider);

    // Initialize labels
    grainSizeLabel.setText("Grain Size:", juce::dontSendNotification);
    grainSizeLabel.attachToComponent(&grainSizeSlider, true);
    addAndMakeVisible(&grainSizeLabel);

    grainOverlapLabel.setText("Grain Overlap:", juce::dontSendNotification);
    grainOverlapLabel.attachToComponent(&grainOverlapSlider, true);
    addAndMakeVisible(&grainOverlapLabel);

    grainSpacingLabel.setText("Grain Spacing:", juce::dontSendNotification);
    grainSpacingLabel.attachToComponent(&grainSpacingSlider, true);
    addAndMakeVisible(&grainSpacingLabel);

    // Load file button
    loadFileButton.setButtonText("Load Audio File");
    loadFileButton.onClick = [this]() { loadFileButtonClicked(); };
    addAndMakeVisible(&loadFileButton);

    // Attach sliders to the AudioProcessorValueTreeState
    grainSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "GRAIN_SIZE", grainSizeSlider);
    grainOverlapAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "GRAIN_OVERLAP", grainOverlapSlider);
    grainSpacingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "GRAIN_SPACING", grainSpacingSlider);

    // Enable drag and drop
    setWantsKeyboardFocus(true);
    setMouseClickGrabsKeyboardFocus(false);
}

Hw5AudioProcessorEditor::~Hw5AudioProcessorEditor()
{
}

void Hw5AudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // Instructions
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Drag and drop an audio file onto the plugin UI to load it.", getLocalBounds(), juce::Justification::centredBottom, 2);
}

void Hw5AudioProcessorEditor::resized()
{
    int labelWidth = 100;
    int sliderHeight = 30;
    int yPosition = 20;

    grainSizeSlider.setBounds(labelWidth, yPosition, getWidth() - labelWidth - 20, sliderHeight);
    yPosition += sliderHeight + 10;

    grainOverlapSlider.setBounds(labelWidth, yPosition, getWidth() - labelWidth - 20, sliderHeight);
    yPosition += sliderHeight + 10;

    grainSpacingSlider.setBounds(labelWidth, yPosition, getWidth() - labelWidth - 20, sliderHeight);
    yPosition += sliderHeight + 20;

    loadFileButton.setBounds((getWidth() - 150) / 2, yPosition, 150, 30);
}

bool Hw5AudioProcessorEditor::isInterestedInFileDrag (const juce::StringArray& files)
{
    // We are interested if any of the files are audio files
    for (auto& file : files)
    {
        if (juce::AudioFormatManager().findFormatForFileExtension(file))
            return true;
    }
    return false;
}

void Hw5AudioProcessorEditor::filesDropped (const juce::StringArray& files, int x, int y)
{
    for (auto& file : files)
    {
        juce::File audioFile(file);

        if (audioFile.existsAsFile())
        {
            audioProcessor.granSynth.loadAudioFile(audioFile);
            break; // Only load the first valid audio file
        }
    }
}

void Hw5AudioProcessorEditor::loadAudioFile()
{
    juce::FileChooser chooser("Select an audio file...", {}, "*.wav;*.mp3;*.aiff;*.flac");

    if (chooser.browseForFileToOpen())
    {
        juce::File audioFile = chooser.getResult();
        audioProcessor.granSynth.loadAudioFile(audioFile);
    }
}

void Hw5AudioProcessorEditor::loadFileButtonClicked()
{
    loadAudioFile();
}
