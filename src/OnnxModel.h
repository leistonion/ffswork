#pragma once

#include <memory>
#include <string>
#include <vector>

#include <onnxruntime_cxx_api.h>

class OnnxModel
{
public:
    bool load(const std::string& modelPath, bool preferCuda);
    bool isLoaded() const noexcept;
    bool process(const float* in, float* out, int numSamples);

private:
    Ort::Env env { ORT_LOGGING_LEVEL_WARNING, "VoiceCleanAI" };
    std::unique_ptr<Ort::Session> session;
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
};
