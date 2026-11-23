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
const juce::Colour surface { 0xfff2f5ef };
const juce::Colour activeSurface { 0xffe5efe7 };
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
NewProjectAudioProcessorEditor::EditorLookAndFeel::EditorLookAndFeel()
{
    setColour (juce::ComboBox::textColourId, ink);
    setColour (juce::PopupMenu::backgroundColourId, panel);
    setColour (juce::PopupMenu::textColourId, ink);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, accent);
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::TooltipWindow::backgroundColourId, ink);
    setColour (juce::TooltipWindow::textColourId, juce::Colours::white);
    setColour (juce::TooltipWindow::outlineColourId, ink);
}

void NewProjectAudioProcessorEditor::EditorLookAndFeel::drawRotarySlider (
    juce::Graphics& g, int x, int y, int width, int height,
    float position, float startAngle, float endAngle, juce::Slider& knob)
{
    const juce::Rectangle<float> bounds (static_cast<float> (x), static_cast<float> (y),
                                        static_cast<float> (width), static_cast<float> (height));
    const auto centre = bounds.getCentre().translated (0.0f, 4.0f);
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.34f;
    const auto angle = juce::jmap (position, startAngle, endAngle);
    const auto steps = juce::roundToInt ((knob.getMaximum() - knob.getMinimum()) / knob.getInterval());
    const auto selected = juce::roundToInt (position * static_cast<float> (steps));

    for (int i = 0; i <= steps; ++i)
    {
        const auto tickAngle = juce::jmap (static_cast<float> (i) / static_cast<float> (steps), startAngle, endAngle);
        g.setColour (i == selected ? accent : mutedInk);
        g.drawLine ({ centre.getPointOnCircumference (radius + 8.0f, tickAngle),
                      centre.getPointOnCircumference (radius + 14.0f, tickAngle) },
                    i == selected ? 3.0f : 1.5f);
    }

    const auto face = juce::Rectangle<float> (radius * 2.0f, radius * 2.0f).withCentre (centre);
    g.setColour (border);
    g.fillEllipse (face.translated (0.0f, 2.0f));
    g.setColour (knob.isMouseOverOrDragging() ? activeSurface : surface);
    g.fillEllipse (face);
    g.setColour (knob.hasKeyboardFocus (false) ? accent : ink);
    g.drawEllipse (face, 1.5f);
    g.setColour (accent);
    g.drawLine ({ centre.getPointOnCircumference (radius * 0.53f, angle),
                  centre.getPointOnCircumference (radius * 0.80f, angle) }, 3.0f);
}

void NewProjectAudioProcessorEditor::EditorLookAndFeel::drawComboBox (
    juce::Graphics& g, int width, int height, bool isButtonDown, int, int, int, int, juce::ComboBox& box)
{
    const juce::Rectangle<float> bounds (static_cast<float> (width), static_cast<float> (height));
    const auto active = isButtonDown || box.isPopupActive() || box.hasKeyboardFocus (true);
    g.setColour (active ? activeSurface : surface);
    g.fillRoundedRectangle (bounds, 8.0f);
    g.setColour (active ? accent : border);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 8.0f, 1.0f);

    const auto arrowX = bounds.getRight() - 16.0f;
    const auto arrowY = bounds.getCentreY();
    juce::Path chevron;
    chevron.startNewSubPath (arrowX - 4.0f, arrowY - 2.0f);
    chevron.lineTo (arrowX, arrowY + 2.0f);
    chevron.lineTo (arrowX + 4.0f, arrowY - 2.0f);
    g.setColour (accent);
    g.strokePath (chevron, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
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
    g.setFont (juce::FontOptions (27.0f, juce::Font::bold));
    g.drawText ("ChordCraft", getLocalBounds().removeFromTop (64).reduced (26, 0),
                juce::Justification::centredLeft);

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
