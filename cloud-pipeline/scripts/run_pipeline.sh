#!/usr/bin/env bash
set -euo pipefail

events="${EVENTS:-100}"
seed="${SEED:-12345}"
chunk_index="${CHUNK_INDEX:-0}"
event_offset="${EVENT_OFFSET:-0}"
chunk_name="$(printf 'chunk_%05d' "$chunk_index")"
output_dir="${OUTPUT_DIR:-output/chunks/$chunk_name}"

mkdir -p "$output_dir"
exec > >(tee "$output_dir/run.log") 2>&1

echo "[Pipeline] Starting chunk $chunk_index"
echo "[Pipeline] Events: $events"
echo "[Pipeline] Global event range: $event_offset-$((event_offset + events - 1))"
echo "[Pipeline] Seed: $seed"
echo "[Pipeline] Output directory: $output_dir"

echo "[Pipeline] Building Geant4 application"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel "$(nproc)"
echo "[Pipeline] Generating PYTHIA events"
python generator/generate_events.py \
  --events "$events" --seed "$seed" --event-offset "$event_offset" \
  --hepmc "$output_dir/events.hepmc" --truth "$output_dir/truth.csv"
echo "[Pipeline] Starting Geant4 transport"
./build/calo_sim \
  --input "$output_dir/events.hepmc" --output "$output_dir/hits.csv" \
  --events "$events" --event-offset "$event_offset"
echo "[Pipeline] Creating analysis outputs"
python analysis/analyze_response.py \
  --truth "$output_dir/truth.csv" --hits "$output_dir/hits.csv" \
  --output-dir "$output_dir/analysis"
python scripts/write_manifest.py \
  --output "$output_dir/completion.json" --chunk-index "$chunk_index" \
  --event-offset "$event_offset" --events "$events" --seed "$seed"
echo "[Pipeline] Chunk $chunk_index completed successfully"
echo "[Pipeline] Persistent log: $output_dir/run.log"
