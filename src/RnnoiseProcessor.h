#pragma once

#include <array>
#include <vector>

#include <rnnoise.h>

class RnnoiseProcessor
{
public:
    void prepare(double sampleRate);
    void reset();
    void processBuffer(float* samples, int numSamples);

private:
    static constexpr int frameSize = 480;
    DenoiseState* state = nullptr;
    double currentSampleRate = 48000.0;
    std::vector<float> fifo;
};
