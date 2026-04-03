# VoiceCleanAI (JUCE VST3)

Production-oriented JUCE VST3 plugin for Windows voiceover cleanup with:

- **RNNoise** low-latency pre-clean stage.
- **ONNX Runtime** inference pipeline for main enhancement + dereverb stages.
- **CMake + Visual Studio 2022** build path.

## Verified open-source components

- **RNNoise** (Xiph) — BSD-3-Clause, frame-based real-time denoising.
- **DeepFilterNet** (Rikorose) — MIT license, real-time speech enhancement.
- **Demucs** (facebookresearch) — MIT license, high quality but heavier for strict low-latency real-time plugin use.

Selected default runtime stack for this plugin:

1. RNNoise
2. DeepFilterNet ONNX (main model)
3. Second ONNX stage for dereverb (defaulted to same ONNX file unless replaced)

## Signal chain

`Input -> STFT -> RNNoise -> Main ONNX model -> Dereverb ONNX model -> wet/dry + voice-preservation mix -> Output`

## Project layout

- `CMakeLists.txt` - JUCE + RNNoise + ONNX Runtime integration and VST3 target.
- `src/PluginProcessor.*` - real-time audio pipeline.
- `src/PluginEditor.*` - JUCE controls for required UX.
- `src/RnnoiseProcessor.*` - RNNoise wrapper.
- `src/OnnxModel.*` - ONNX Runtime model loader and inference.
- `src/StftProcessor.*` - STFT preprocessing stage.
- `scripts/setup_windows.ps1` - downloads ONNX Runtime and DeepFilterNet ONNX package.

## Windows build (Visual Studio 2022)

### 1) Download dependencies + models

Run in PowerShell:

```powershell
./scripts/setup_windows.ps1
```

This script downloads:

- ONNX Runtime `v1.18.1` (Windows x64 prebuilt).
- DeepFilterNet3 ONNX tarball.
- Initializes `models/deepfilternet3.onnx` and `models/dereverb.onnx`.

### 2) Configure CMake (VS 2022)

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64 -DONNXRUNTIME_ROOT="<repo>\third_party\onnxruntime-win-x64-1.18.1"
```

### 3) Build Release

```powershell
cmake --build build --config Release
```

### 4) Expected plugin artifact

```text
build/VoiceCleanAI_artefacts/Release/VST3/VoiceCleanAI.vst3
```

## Runtime notes

- RNNoise stage is optimized for 48 kHz stream blocks.
- ONNX stage expects waveform input tensor shaped `[1, 1, T]`.
- Enable GPU from UI toggle only if ONNX Runtime CUDA build is used.

## Controls

- Noise Reduction (0-100%)
- Reverb Reduction (0-100%)
- Fine Tune (0-100%)
- Voice Preservation
- Attack / Release smoothing controls
- GPU toggle

## Replacing dereverb model

The dereverb stage is independently loaded from `models/dereverb.onnx`.
Replace that file with a dedicated speech dereverberation ONNX model to improve room-echo suppression quality.
