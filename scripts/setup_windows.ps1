$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $PSScriptRoot
$ThirdParty = Join-Path $Root 'third_party'
$Models = Join-Path $Root 'models'

New-Item -ItemType Directory -Force -Path $ThirdParty | Out-Null
New-Item -ItemType Directory -Force -Path $Models | Out-Null

$OrtVersion = '1.18.1'
$OrtZip = Join-Path $ThirdParty "onnxruntime-win-x64-$OrtVersion.zip"
$OrtUrl = "https://github.com/microsoft/onnxruntime/releases/download/v$OrtVersion/onnxruntime-win-x64-$OrtVersion.zip"

Invoke-WebRequest -Uri $OrtUrl -OutFile $OrtZip
Expand-Archive -Path $OrtZip -DestinationPath $ThirdParty -Force

$DfUrl = 'https://huggingface.co/niobures/DeepFilterNet/resolve/main/models/onnx/DeepFilterNet/DeepFilterNet3_onnx.tar.gz'
$DfTar = Join-Path $Models 'DeepFilterNet3_onnx.tar.gz'
Invoke-WebRequest -Uri $DfUrl -OutFile $DfTar

tar -xzf $DfTar -C $Models

# Expected generated filename from DeepFilterNet export.
if (Test-Path (Join-Path $Models 'model.onnx')) {
    Move-Item -Force (Join-Path $Models 'model.onnx') (Join-Path $Models 'deepfilternet3.onnx')
}

# Start with the same model as dereverb baseline, replace with dedicated dereverb ONNX as desired.
if (Test-Path (Join-Path $Models 'deepfilternet3.onnx')) {
    Copy-Item -Force (Join-Path $Models 'deepfilternet3.onnx') (Join-Path $Models 'dereverb.onnx')
}

Write-Host 'Setup complete.'
Write-Host "ONNXRUNTIME_ROOT=$ThirdParty/onnxruntime-win-x64-$OrtVersion"
