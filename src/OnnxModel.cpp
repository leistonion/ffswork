#include "OnnxModel.h"

#include <cstring>

bool OnnxModel::load(const std::string& modelPath, bool preferCuda)
{
    Ort::SessionOptions options;
    options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

    (void) preferCuda;

    try
    {
        session = std::make_unique<Ort::Session>(env, modelPath.c_str(), options);
    }
    catch (...)
    {
        session.reset();
        return false;
    }

    Ort::AllocatorWithDefaultOptions allocator;
    inputNames.clear();
    outputNames.clear();

    const size_t inCount = session->GetInputCount();
    const size_t outCount = session->GetOutputCount();

    for (size_t i = 0; i < inCount; ++i)
        inputNames.emplace_back(session->GetInputNameAllocated(i, allocator).get());

    for (size_t i = 0; i < outCount; ++i)
        outputNames.emplace_back(session->GetOutputNameAllocated(i, allocator).get());

    return !inputNames.empty() && !outputNames.empty();
}

bool OnnxModel::isLoaded() const noexcept
{
    return session != nullptr;
}

bool OnnxModel::process(const float* in, float* out, int numSamples)
{
    if (! isLoaded())
    {
        std::memcpy(out, in, static_cast<size_t>(numSamples) * sizeof(float));
        return false;
    }

    std::vector<int64_t> shape { 1, 1, static_cast<int64_t>(numSamples) };
    auto inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, const_cast<float*>(in), static_cast<size_t>(numSamples), shape.data(), shape.size());

    std::vector<const char*> inNames;
    std::vector<const char*> outNames;
    inNames.reserve(inputNames.size());
    outNames.reserve(outputNames.size());

    for (auto& n : inputNames) inNames.push_back(n.c_str());
    for (auto& n : outputNames) outNames.push_back(n.c_str());

    auto output = session->Run(Ort::RunOptions{ nullptr }, inNames.data(), &inputTensor, 1, outNames.data(), 1);

    if (output.empty() || ! output[0].IsTensor())
    {
        std::memcpy(out, in, static_cast<size_t>(numSamples) * sizeof(float));
        return false;
    }

    const float* data = output[0].GetTensorData<float>();
    std::memcpy(out, data, static_cast<size_t>(numSamples) * sizeof(float));
    return true;
}
