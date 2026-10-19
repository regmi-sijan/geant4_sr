#!/usr/bin/env bash
set -euo pipefail

events="${EVENTS:-100}"
seed="${SEED:-12345}"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel "$(nproc)"
mkdir -p output
python generator/generate_events.py --events "$events" --seed "$seed"
./build/calo_sim --input output/events.hepmc --output output/hits.csv --events "$events"
python analysis/analyze_response.py
