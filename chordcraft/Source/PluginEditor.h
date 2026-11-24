/*
  ==============================================================================

    Este archivo define el editor del plugin ChordCraft.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class NewProjectAudioProcessor;

//==============================================================================
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class Knob final : public juce::Slider
    {
        void paintOverChildren (juce::Graphics&) override;
        void focusGained (FocusChangeType) override { repaint(); }
        void focusLost (FocusChangeType) override { repaint(); }
    };

    juce::Rectangle<int> getPanelBounds() const;

    juce::LookAndFeel_V2 editorLookAndFeel;

    juce::Label amplitudeTitle, complexityTitle, scaleTitle;
    Knob amplitudeKnob, complexityKnob;
    juce::Label amplitudeValue, complexityValue;
    juce::Label baseTitle, modeTitle;
    juce::ComboBox baseSelector, modeSelector;
    juce::TooltipWindow tooltips { this, 550 };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amplitudeAttachment, complexityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> baseAttachment, modeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
