#!/usr/bin/env bash
set -euo pipefail

repository="regmi-sijan/geant4_sr"
workflow="cloud-simulation.yml"
chunk_index="${1:-0}"
events_per_chunk="${2:-20}"
base_seed="${3:-12345}"

if [[ "$chunk_index" == "-h" || "$chunk_index" == "--help" ]]; then
  echo "Usage: $0 [chunk_index] [events_per_chunk] [base_seed]"
  echo "Defaults: chunk_index=0, events_per_chunk=20, base_seed=12345"
  echo "Example:  $0 3 20"
  exit 0
fi

if ! [[ "$chunk_index" =~ ^[0-9]+$ ]]; then
  echo "chunk_index must be a non-negative integer" >&2
  exit 2
fi

if ! [[ "$events_per_chunk" =~ ^[1-9][0-9]*$ ]]; then
  echo "events_per_chunk must be a positive integer" >&2
  exit 2
fi

if ! [[ "$base_seed" =~ ^[1-9][0-9]*$ ]] || (( base_seed > 900000000 )); then
  echo "base_seed must be between 1 and 900000000" >&2
  exit 2
fi

if ! command -v gh >/dev/null 2>&1; then
  echo "GitHub CLI (gh) is not installed" >&2
  exit 127
fi

gh auth status >/dev/null

echo "Starting cloud simulation:"
echo "  chunk index:     $chunk_index"
echo "  events per chunk: $events_per_chunk"
echo "  base seed:       $base_seed"

gh workflow run "$workflow" \
  --repo "$repository" \
  -f "chunk_index=$chunk_index" \
  -f "events_per_chunk=$events_per_chunk" \
  -f "base_seed=$base_seed"

echo "Workflow submitted. View it at:"
echo "https://github.com/$repository/actions"
echo
echo "Monitor the newest workflow from this Terminal with:"
echo "gh run watch --repo $repository --exit-status"
