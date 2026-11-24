/*
  ==============================================================================

    Este archivo contiene el código base del procesador de un plugin de JUCE.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ChordEngine.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static constexpr auto baseParameter = "base";
    static constexpr auto modeParameter = "mode";
    static constexpr auto amplitudeParameter = "amplitude";
    static constexpr auto complexityParameter = "complexity";

    juce::AudioProcessorValueTreeState& getParametersState() noexcept { return parameters; }

private:
    void triggerChord (int note, int channel, juce::uint8 velocity, int samplePosition);
    void stopActiveChord (int samplePosition);

    juce::AudioProcessorValueTreeState parameters;
    const std::atomic<float>* const baseValue;
    const std::atomic<float>* const modeValue;
    const std::atomic<float>* const amplitudeValue;
    const std::atomic<float>* const complexityValue;

    juce::MidiBuffer output;
    chordcraft::Chord activeChord;
    int activeTriggerNote = -1;
    int activeChannel = 1;
    std::atomic<bool> resetPending { false };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};
