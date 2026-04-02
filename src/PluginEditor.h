#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class VoiceCleanAIAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit VoiceCleanAIAudioProcessorEditor(VoiceCleanAIAudioProcessor&);
    ~VoiceCleanAIAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    VoiceCleanAIAudioProcessor& processor;

    juce::Slider noiseSlider, reverbSlider, fineSlider, voiceSlider, attackSlider, releaseSlider;
    juce::ToggleButton gpuButton { "GPU" };

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> noiseAttach, reverbAttach, fineAttach, voiceAttach, attackAttach, releaseAttach;
    std::unique_ptr<ButtonAttachment> gpuAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoiceCleanAIAudioProcessorEditor)
};
