/*
  ==============================================================================

    This file defines the ChordCraft plugin editor.

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
    class EditorLookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        EditorLookAndFeel();

        void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                               float position, float startAngle, float endAngle, juce::Slider&) override;
        void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                           int buttonX, int buttonY, int buttonWidth, int buttonHeight, juce::ComboBox&) override;
    };

    class Knob final : public juce::Slider
    {
        void focusGained (FocusChangeType) override { repaint(); }
        void focusLost (FocusChangeType) override { repaint(); }
    };

    juce::Rectangle<int> getPanelBounds() const;

    EditorLookAndFeel editorLookAndFeel;

    juce::Label amplitudeTitle, complexityTitle, scaleTitle;
    Knob amplitudeKnob, complexityKnob;
    juce::Label amplitudeValue, complexityValue;
    juce::Label baseTitle, modeTitle;
    juce::ComboBox baseSelector, modeSelector;
    juce::TooltipWindow tooltips { this, 550 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
