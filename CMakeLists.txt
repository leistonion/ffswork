cmake_minimum_required(VERSION 3.24)

project(VoiceCleanAI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

# JUCE
FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.4
)
FetchContent_MakeAvailable(juce)

# RNNoise
FetchContent_Declare(
  rnnoise
  GIT_REPOSITORY https://github.com/xiph/rnnoise.git
  GIT_TAG main
)
FetchContent_MakeAvailable(rnnoise)

if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(ONNXRUNTIME_ROOT "" CACHE PATH "Path to ONNX Runtime prebuilt directory (contains include/ and lib/)")
if (NOT ONNXRUNTIME_ROOT)
  message(FATAL_ERROR "Set ONNXRUNTIME_ROOT to your ONNX Runtime directory. See scripts/setup_windows.ps1")
endif()

add_library(onnxruntime SHARED IMPORTED GLOBAL)
set_target_properties(onnxruntime PROPERTIES
  IMPORTED_IMPLIB "${ONNXRUNTIME_ROOT}/lib/onnxruntime.lib"
  IMPORTED_LOCATION "${ONNXRUNTIME_ROOT}/lib/onnxruntime.dll"
  INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_ROOT}/include"
)

juce_add_plugin(VoiceCleanAI
  COMPANY_NAME "VoiceCleanAI"
  PLUGIN_MANUFACTURER_CODE VcAI
  PLUGIN_CODE VcDn
  FORMATS VST3
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  PRODUCT_NAME "VoiceCleanAI")

target_sources(VoiceCleanAI PRIVATE
  src/PluginProcessor.cpp
  src/PluginProcessor.h
  src/PluginEditor.cpp
  src/PluginEditor.h
  src/OnnxModel.h
  src/OnnxModel.cpp
  src/RnnoiseProcessor.h
  src/RnnoiseProcessor.cpp
  src/StftProcessor.h
  src/StftProcessor.cpp
)

target_include_directories(VoiceCleanAI PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(VoiceCleanAI PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  rnnoise
  onnxruntime
)

target_compile_definitions(VoiceCleanAI
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_generate_juce_header(VoiceCleanAI)
