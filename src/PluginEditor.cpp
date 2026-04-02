#include "PluginEditor.h"

namespace
{
void setupSlider(juce::Slider& s, const juce::String& name)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    s.setName(name);
}
}

VoiceCleanAIAudioProcessorEditor::VoiceCleanAIAudioProcessorEditor(VoiceCleanAIAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setupSlider(noiseSlider, "Noise Reduction");
    setupSlider(reverbSlider, "Reverb Reduction");
    setupSlider(fineSlider, "Fine Tune");
    setupSlider(voiceSlider, "Voice Preservation");
    setupSlider(attackSlider, "Attack");
    setupSlider(releaseSlider, "Release");

    addAndMakeVisible(noiseSlider);
    addAndMakeVisible(reverbSlider);
    addAndMakeVisible(fineSlider);
    addAndMakeVisible(voiceSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(gpuButton);

    noiseAttach = std::make_unique<SliderAttachment>(processor.apvts, "noise", noiseSlider);
    reverbAttach = std::make_unique<SliderAttachment>(processor.apvts, "reverb", reverbSlider);
    fineAttach = std::make_unique<SliderAttachment>(processor.apvts, "fine", fineSlider);
    voiceAttach = std::make_unique<SliderAttachment>(processor.apvts, "voice", voiceSlider);
    attackAttach = std::make_unique<SliderAttachment>(processor.apvts, "attack", attackSlider);
    releaseAttach = std::make_unique<SliderAttachment>(processor.apvts, "release", releaseSlider);
    gpuAttach = std::make_unique<ButtonAttachment>(processor.apvts, "gpu", gpuButton);

    setSize(640, 360);
}

void VoiceCleanAIAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1c1e24));
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("VoiceCleanAI", 0, 8, getWidth(), 32, juce::Justification::centred, 1);
}

void VoiceCleanAIAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16).withTrimmedTop(40);
    auto top = area.removeFromTop(150);

    auto row1w = top.getWidth() / 3;
    noiseSlider.setBounds(top.removeFromLeft(row1w).reduced(8));
    reverbSlider.setBounds(top.removeFromLeft(row1w).reduced(8));
    fineSlider.setBounds(top.reduced(8));

    auto bottom = area.removeFromTop(150);
    auto row2w = bottom.getWidth() / 3;
    voiceSlider.setBounds(bottom.removeFromLeft(row2w).reduced(8));
    attackSlider.setBounds(bottom.removeFromLeft(row2w).reduced(8));
    releaseSlider.setBounds(bottom.reduced(8));

    gpuButton.setBounds(area.removeFromTop(30).removeFromLeft(80));
}
