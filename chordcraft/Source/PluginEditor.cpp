/*
  ==============================================================================

    This file implements the ChordCraft plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
const juce::Colour background { 0xffebece5 };
const juce::Colour panel { 0xfffdfdf9 };
const juce::Colour border { 0xffd5dcd2 };
const juce::Colour ink { 0xff1c2923 };
const juce::Colour mutedInk { 0xff526358 };
const juce::Colour accent { 0xff17624b };

void styleLabel (juce::Label& label, const juce::String& text, float size, juce::Colour colour,
                 juce::Justification justification, int style = juce::Font::plain)
{
    label.setText (text, juce::dontSendNotification);
    label.setFont (juce::FontOptions (size, style));
    label.setColour (juce::Label::textColourId, colour);
    label.setJustificationType (justification);
    label.setBorderSize (juce::BorderSize<int> (0));
}

void configureKnob (juce::Slider& knob, const juce::String& title, const juce::String& tooltip)
{
    constexpr auto pi = juce::MathConstants<float>::pi;
    knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setRotaryParameters (1.6f * pi, 2.4f * pi, true);
    knob.setMouseDragSensitivity (120);
    knob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    knob.setTitle (title);
    knob.setTooltip (tooltip);
    knob.setWantsKeyboardFocus (true);
    knob.setMouseClickGrabsKeyboardFocus (true);
}
} // namespace

//==============================================================================
void NewProjectAudioProcessorEditor::Knob::paintOverChildren (juce::Graphics& g)
{
    if (hasKeyboardFocus (false))
    {
        g.setColour (accent);
        g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (1.0f), 6.0f, 1.5f);
    }
}

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p)
{
    setOpaque (true);
    setLookAndFeel (&editorLookAndFeel);

    styleLabel (amplitudeTitle, "Amplitud", 17.0f, ink, juce::Justification::centred, juce::Font::bold);
    styleLabel (complexityTitle, "Complejidad", 17.0f, ink, juce::Justification::centred, juce::Font::bold);
    styleLabel (scaleTitle, "Escala", 17.0f, ink, juce::Justification::centred, juce::Font::bold);
    styleLabel (baseTitle, "Base", 14.0f, mutedInk, juce::Justification::centredLeft);
    styleLabel (modeTitle, "Modo", 14.0f, mutedInk, juce::Justification::centredLeft);
    styleLabel (amplitudeValue, {}, 15.0f, accent, juce::Justification::centred, juce::Font::bold);
    styleLabel (complexityValue, {}, 15.0f, accent, juce::Justification::centred, juce::Font::bold);

    configureKnob (amplitudeKnob, "Amplitud", "Ajusta la distancia entre las notas del acorde.");
    configureKnob (complexityKnob, "Complejidad",
                   juce::String::fromUTF8 ("Ajusta la riqueza armónica del acorde en cinco niveles."));

    amplitudeKnob.onValueChange = [this]
    {
        amplitudeValue.setText (amplitudeKnob.getTextFromValue (amplitudeKnob.getValue()), juce::dontSendNotification);
    };
    complexityKnob.onValueChange = [this]
    {
        complexityValue.setText ("Nivel " + juce::String (juce::roundToInt (complexityKnob.getValue())),
                                 juce::dontSendNotification);
    };
    baseSelector.setTitle ("Base de la escala");
    baseSelector.setTooltip (juce::String::fromUTF8 ("Selecciona la tónica de la escala."));
    for (std::size_t i = 0; i < chordcraft::noteNames.size(); ++i)
        baseSelector.addItem (chordcraft::noteNames[i], static_cast<int> (i) + 1);

    modeSelector.setTitle ("Modo de la escala");
    modeSelector.setTooltip ("Selecciona el modo mayor o menor natural.");
    for (std::size_t i = 0; i < chordcraft::modeNames.size(); ++i)
        modeSelector.addItem (chordcraft::modeNames[i], static_cast<int> (i) + 1);

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    auto& state = p.getParametersState();
    amplitudeAttachment = std::make_unique<SliderAttachment> (state, p.amplitudeParameter, amplitudeKnob);
    complexityAttachment = std::make_unique<SliderAttachment> (state, p.complexityParameter, complexityKnob);
    baseAttachment = std::make_unique<ComboBoxAttachment> (state, p.baseParameter, baseSelector);
    modeAttachment = std::make_unique<ComboBoxAttachment> (state, p.modeParameter, modeSelector);
    amplitudeKnob.onValueChange();
    complexityKnob.onValueChange();

    for (auto* label : { &amplitudeTitle, &complexityTitle, &scaleTitle, &baseTitle, &modeTitle,
                        &amplitudeValue, &complexityValue })
        addAndMakeVisible (*label);

    addAndMakeVisible (amplitudeKnob);
    addAndMakeVisible (complexityKnob);
    addAndMakeVisible (baseSelector);
    addAndMakeVisible (modeSelector);

    setSize (664, 276);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (background);
    g.setColour (ink);
    g.setFont (juce::FontOptions (27.0f, juce::Font::bold).withFallbacks ({ "Apple Color Emoji",
                                                                         "Segoe UI Emoji",
                                                                         "Noto Color Emoji",
                                                                         "Noto Sans Symbols2" }));
    g.drawText (juce::String::fromUTF8 ("ChordCraft ⛏"), getLocalBounds().removeFromTop (64),
                juce::Justification::centred);

    const auto panelBounds = getPanelBounds().toFloat();
    g.setColour (panel);
    g.fillRoundedRectangle (panelBounds, 16.0f);
    g.setColour (border);
    g.drawRoundedRectangle (panelBounds.reduced (0.5f), 16.0f, 1.0f);

    const auto controls = panelBounds.reduced (15.0f, 20.0f);
    for (int i : { 1, 2 })
        g.drawVerticalLine (juce::roundToInt (controls.getX() + controls.getWidth() * i / 3.0f),
                            controls.getY(), controls.getBottom());
}

void NewProjectAudioProcessorEditor::resized()
{
    auto controls = getPanelBounds().reduced (15, 14);
    const auto columnWidth = controls.getWidth() / 3;

    const auto layoutKnob = [] (juce::Rectangle<int> column, juce::Label& title, juce::Slider& knob, juce::Label& value)
    {
        title.setBounds (column.removeFromTop (24));
        knob.setBounds (column.removeFromTop (108).withSizeKeepingCentre (132, 108));
        value.setBounds (column.removeFromTop (24));
    };

    layoutKnob (controls.removeFromLeft (columnWidth), amplitudeTitle, amplitudeKnob, amplitudeValue);
    layoutKnob (controls.removeFromLeft (columnWidth), complexityTitle, complexityKnob, complexityValue);

    scaleTitle.setBounds (controls.removeFromTop (24));
    auto fields = controls.withSizeKeepingCentre (154, 86);
    auto baseRow = fields.removeFromTop (36);
    baseTitle.setBounds (baseRow.removeFromLeft (48));
    baseSelector.setBounds (baseRow);
    auto modeRow = fields.removeFromBottom (36);
    modeTitle.setBounds (modeRow.removeFromLeft (48));
    modeSelector.setBounds (modeRow);
}

juce::Rectangle<int> NewProjectAudioProcessorEditor::getPanelBounds() const
{
    auto bounds = getLocalBounds().reduced (18);
    bounds.removeFromTop (46);
    return bounds;
}
