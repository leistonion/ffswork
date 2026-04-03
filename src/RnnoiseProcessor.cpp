#include "RnnoiseProcessor.h"

#include <algorithm>

void RnnoiseProcessor::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    if (state == nullptr)
        state = rnnoise_create(nullptr);

    fifo.clear();
    fifo.reserve(frameSize * 2);
}

void RnnoiseProcessor::reset()
{
    if (state != nullptr)
    {
        rnnoise_destroy(state);
        state = nullptr;
    }

    fifo.clear();
}

void RnnoiseProcessor::processBuffer(float* samples, int numSamples)
{
    if (state == nullptr)
        return;

    if (currentSampleRate != 48000.0)
        return;

    int index = 0;
    while (index < numSamples)
    {
        fifo.push_back(samples[index++]);

        if (static_cast<int>(fifo.size()) == frameSize)
        {
            std::array<float, frameSize> frame{};
            std::copy(fifo.begin(), fifo.end(), frame.begin());
            rnnoise_process_frame(state, frame.data(), frame.data());

            for (int i = 0; i < frameSize; ++i)
                samples[index - frameSize + i] = frame[static_cast<size_t>(i)];

            fifo.clear();
        }
    }
}
