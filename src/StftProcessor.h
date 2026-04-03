#pragma once

#include <juce_dsp/juce_dsp.h>

class StftProcessor
{
public:
    void prepare(int fftOrder);
    void processInPlace(float* samples, int numSamples);

private:
    int order = 10;
    int fftSize = 1 << order;
    juce::dsp::FFT fft { order };
    juce::dsp::WindowingFunction<float> window { static_cast<size_t>(fftSize), juce::dsp::WindowingFunction<float>::hann };
    juce::HeapBlock<float> fftBuffer;
};
