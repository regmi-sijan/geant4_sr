#!/usr/bin/env bash
set -euo pipefail

repository="regmi-sijan/geant4_sr"
workflow="cloud-simulation.yml"
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(cd "$script_dir/.." && pwd)"

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
  echo "Usage: $0 [run_id] [download_directory]"
  echo
  echo "With no run_id, downloads the latest successful cloud simulation."
  echo "Default directory: <project>/geant4-results/run-<run_id>"
  exit 0
fi

if ! command -v gh >/dev/null 2>&1; then
  echo "GitHub CLI (gh) is not installed" >&2
  exit 127
fi

gh auth status >/dev/null

run_id="${1:-}"
if [[ -z "$run_id" ]]; then
  echo "Finding the latest successful cloud simulation..."
  run_id="$(gh run list \
    --repo "$repository" \
    --workflow "$workflow" \
    --status success \
    --limit 1 \
    --json databaseId \
    --jq '.[0].databaseId')"
fi

if ! [[ "$run_id" =~ ^[0-9]+$ ]]; then
  echo "No valid successful workflow run was found" >&2
  exit 1
fi

download_dir="${2:-$project_dir/geant4-results/run-$run_id}"
mkdir -p "$download_dir"

echo "Downloading result artifact from run $run_id..."
gh run download "$run_id" \
  --repo "$repository" \
  --pattern 'calorimeter-chunk-*' \
  --dir "$download_dir"

echo
echo "Download complete: $download_dir"
echo "Result files:"
find "$download_dir" -type f -print | sort

echo
echo "Persistent log file(s):"
find "$download_dir" -name run.log -type f -print | sort
