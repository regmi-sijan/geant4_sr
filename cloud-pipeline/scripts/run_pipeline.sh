#!/usr/bin/env bash
set -euo pipefail

events="${EVENTS:-100}"
seed="${SEED:-12345}"
chunk_index="${CHUNK_INDEX:-0}"
event_offset="${EVENT_OFFSET:-0}"
chunk_name="$(printf 'chunk_%05d' "$chunk_index")"
output_dir="${OUTPUT_DIR:-output/chunks/$chunk_name}"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel "$(nproc)"
mkdir -p "$output_dir"
python generator/generate_events.py \
  --events "$events" --seed "$seed" --event-offset "$event_offset" \
  --hepmc "$output_dir/events.hepmc" --truth "$output_dir/truth.csv"
./build/calo_sim \
  --input "$output_dir/events.hepmc" --output "$output_dir/hits.csv" \
  --events "$events" --event-offset "$event_offset"
python analysis/analyze_response.py \
  --truth "$output_dir/truth.csv" --hits "$output_dir/hits.csv" \
  --output-dir "$output_dir/analysis"
python scripts/write_manifest.py \
  --output "$output_dir/completion.json" --chunk-index "$chunk_index" \
  --event-offset "$event_offset" --events "$events" --seed "$seed"
