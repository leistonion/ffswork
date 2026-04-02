#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
OUT_ZIP="${1:-$ROOT_DIR/VoiceCleanAI.zip}"

cd "$ROOT_DIR"
rm -f "$OUT_ZIP"
zip -r "$OUT_ZIP" . \
  -x ".git/*" \
  -x "build/*" \
  -x "build_vs/*" \
  -x "*.zip"

echo "Created: $OUT_ZIP"
