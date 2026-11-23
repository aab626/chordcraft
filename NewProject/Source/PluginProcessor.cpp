/*
  ==============================================================================

    This file implements the ChordCraft plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using Processor = NewProjectAudioProcessor;
    using Choice = juce::AudioParameterChoice;

    return {
        std::make_unique<Choice> (juce::ParameterID { Processor::baseParameter, 1 }, "Base",
                                  juce::StringArray (chordcraft::noteNames.data(), chordcraft::noteNames.size()), 0),
        std::make_unique<Choice> (juce::ParameterID { Processor::modeParameter, 1 }, "Modo",
                                  juce::StringArray (chordcraft::modeNames.data(), chordcraft::modeNames.size()), 0),
        std::make_unique<Choice> (juce::ParameterID { Processor::amplitudeParameter, 1 }, "Amplitud",
                                  juce::StringArray (chordcraft::amplitudeNames.data(), chordcraft::amplitudeNames.size()),
                                  static_cast<int> (chordcraft::Amplitude::closed)),
        std::make_unique<juce::AudioParameterInt> (juce::ParameterID { Processor::complexityParameter, 1 },
                                                  "Complejidad", 1, 5, 1)
    };
}
} // namespace

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
    : AudioProcessor (BusesProperties()),
      parameters (*this, nullptr, "ChordCraft", createParameterLayout()),
      baseValue (parameters.getRawParameterValue (baseParameter)),
      modeValue (parameters.getRawParameterValue (modeParameter)),
      amplitudeValue (parameters.getRawParameterValue (amplitudeParameter)),
      complexityValue (parameters.getRawParameterValue (complexityParameter))
{
}

NewProjectAudioProcessor::~NewProjectAudioProcessor() = default;

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const { return JucePlugin_WantsMidiInput != 0; }
bool NewProjectAudioProcessor::producesMidi() const { return JucePlugin_ProducesMidiOutput != 0; }
bool NewProjectAudioProcessor::isMidiEffect() const { return JucePlugin_IsMidiEffect != 0; }
double NewProjectAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int NewProjectAudioProcessor::getNumPrograms() { return 1; }
int NewProjectAudioProcessor::getCurrentProgram() { return 0; }
void NewProjectAudioProcessor::setCurrentProgram (int) {}
const juce::String NewProjectAudioProcessor::getProgramName (int) { return {}; }
void NewProjectAudioProcessor::changeProgramName (int, const juce::String&) {}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double, int)
{
    output.ensureSize (16384);
    reset();
}

void NewProjectAudioProcessor::releaseResources() {}

void NewProjectAudioProcessor::reset()
{
    resetPending.store (true, std::memory_order_relaxed);
}

bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.inputBuses.isEmpty() && layouts.outputBuses.isEmpty();
}

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    output.clear();

    if (buffer.getNumSamples() > 0 && resetPending.exchange (false, std::memory_order_relaxed))
        stopActiveChord (0);

    for (const auto event : midiMessages)
    {
        if (event.numBytes == 3)
        {
            const auto status = event.data[0] & 0xf0;
            const auto channel = (event.data[0] & 0x0f) + 1;

            if (status == 0x90 || status == 0x80)
            {
                const auto note = event.data[1];
                if (status == 0x90 && event.data[2] != 0)
                    triggerChord (note, channel, event.data[2], event.samplePosition);
                else if (note == activeTriggerNote && channel == activeChannel)
                    stopActiveChord (event.samplePosition);

                continue;
            }

            constexpr int allSoundOff = 120;
            constexpr int allNotesOff = 123;
            if (status == 0xb0 && channel == activeChannel && (event.data[1] == allSoundOff || event.data[1] == allNotesOff))
                stopActiveChord (event.samplePosition);
        }

        output.addEvent (event.data, event.numBytes, event.samplePosition);
    }

    midiMessages.clear();
    midiMessages.addEvents (output, 0, -1, 0);
}

void NewProjectAudioProcessor::triggerChord (int note, int channel, juce::uint8 velocity, int samplePosition)
{
    if (chordcraft::degreeForMidiNote (note) < 0)
        return;

    const auto base = static_cast<int> (baseValue->load (std::memory_order_relaxed));
    const auto mode = static_cast<chordcraft::ScaleMode> (static_cast<int> (modeValue->load (std::memory_order_relaxed)));
    const auto amplitude = static_cast<chordcraft::Amplitude> (static_cast<int> (amplitudeValue->load (std::memory_order_relaxed)));
    const auto complexity = static_cast<int> (complexityValue->load (std::memory_order_relaxed));

    stopActiveChord (samplePosition);
    activeChord = chordcraft::makeChord (note, base, mode, complexity, amplitude);
    activeTriggerNote = note;
    activeChannel = channel;

    for (const auto pitch : activeChord)
        output.addEvent (juce::MidiMessage::noteOn (channel, pitch, velocity), samplePosition);
}

void NewProjectAudioProcessor::stopActiveChord (int samplePosition)
{
    for (const auto pitch : activeChord)
        output.addEvent (juce::MidiMessage::noteOff (activeChannel, pitch), samplePosition);

    activeChord.size = 0;
    activeTriggerNote = -1;
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (const auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xml = getXmlFromBinary (data, sizeInBytes);
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
