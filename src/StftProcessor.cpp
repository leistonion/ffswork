#include "StftProcessor.h"

#include <algorithm>

void StftProcessor::prepare(int fftOrder)
{
    order = fftOrder;
    fftSize = 1 << order;
    fft = juce::dsp::FFT(order);
    window = juce::dsp::WindowingFunction<float>(static_cast<size_t>(fftSize), juce::dsp::WindowingFunction<float>::hann);
    fftBuffer.allocate(static_cast<size_t>(2 * fftSize), true);
}

void StftProcessor::processInPlace(float* samples, int numSamples)
{
    if (numSamples < fftSize)
        return;

    std::fill(fftBuffer.get(), fftBuffer.get() + (2 * fftSize), 0.0f);
    std::copy(samples, samples + fftSize, fftBuffer.get());
    window.multiplyWithWindowingTable(fftBuffer.get(), static_cast<size_t>(fftSize));
    fft.performRealOnlyForwardTransform(fftBuffer.get());
    fft.performRealOnlyInverseTransform(fftBuffer.get());

    const float scale = 1.0f / static_cast<float>(fftSize);
    for (int i = 0; i < fftSize; ++i)
        samples[i] = fftBuffer[i] * scale;
}
