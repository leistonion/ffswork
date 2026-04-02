#include "PluginProcessor.h"
#include "PluginEditor.h"

VoiceCleanAIAudioProcessor::VoiceCleanAIAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createParams())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout VoiceCleanAIAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("noise", "Noise Reduction", 0.0f, 100.0f, 60.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverb", "Reverb Reduction", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fine", "Fine Tune", 0.0f, 100.0f, 45.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("voice", "Voice Preservation", 0.0f, 100.0f, 70.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 1.0f, 200.0f, 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", 10.0f, 500.0f, 120.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("gpu", "Use GPU", false));
    return { params.begin(), params.end() };
}

void VoiceCleanAIAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    rnnoise.prepare(sampleRate);
    stft.prepare(10);

    monoScratch.setSize(1, samplesPerBlock);
    modelScratch.setSize(1, samplesPerBlock);

    const auto modelPath = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                               .getParentDirectory()
                               .getChildFile("models")
                               .getChildFile("deepfilternet3.onnx")
                               .getFullPathName()
                               .toStdString();

    const auto dereverbPath = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                                  .getParentDirectory()
                                  .getChildFile("models")
                                  .getChildFile("dereverb.onnx")
                                  .getFullPathName()
                                  .toStdString();

    const bool useGpu = apvts.getRawParameterValue("gpu")->load() > 0.5f;

    if (! mainModel.isLoaded())
        mainModel.load(modelPath, useGpu);

    if (! dereverbModel.isLoaded())
        dereverbModel.load(dereverbPath, useGpu);
}

void VoiceCleanAIAudioProcessor::releaseResources()
{
    rnnoise.reset();
}

bool VoiceCleanAIAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet()
           && (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
               || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo());
}

void VoiceCleanAIAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    monoScratch.setSize(1, numSamples, false, false, true);
    modelScratch.setSize(1, numSamples, false, false, true);

    auto* mono = monoScratch.getWritePointer(0);
    std::fill(mono, mono + numSamples, 0.0f);

    for (int ch = 0; ch < numChannels; ++ch)
        juce::FloatVectorOperations::add(mono, buffer.getReadPointer(ch), numSamples);

    juce::FloatVectorOperations::multiply(mono, 1.0f / static_cast<float>(numChannels), numSamples);

    stft.processInPlace(mono, numSamples);
    rnnoise.processBuffer(mono, numSamples);

    auto* cleaned = modelScratch.getWritePointer(0);
    mainModel.process(mono, cleaned, numSamples);
    dereverbModel.process(cleaned, mono, numSamples);

    const float noiseAmt = apvts.getRawParameterValue("noise")->load() / 100.0f;
    const float revAmt = apvts.getRawParameterValue("reverb")->load() / 100.0f;
    const float voicePreserve = apvts.getRawParameterValue("voice")->load() / 100.0f;
    const float wet = juce::jlimit(0.0f, 1.0f, (noiseAmt * 0.5f + revAmt * 0.5f) * (1.0f - 0.4f * voicePreserve));

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* out = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i)
            out[i] = juce::jmap(wet, out[i], mono[i]);
    }
}

void VoiceCleanAIAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VoiceCleanAIAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorEditor* VoiceCleanAIAudioProcessor::createEditor()
{
    return new VoiceCleanAIAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VoiceCleanAIAudioProcessor();
}
