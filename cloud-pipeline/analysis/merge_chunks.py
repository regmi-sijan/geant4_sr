"""Merge completed simulation chunks and regenerate combined analysis."""

import argparse
import json
import subprocess
import sys
from pathlib import Path

import pandas as pd


parser = argparse.ArgumentParser()
parser.add_argument("--chunks", type=Path, default=Path("output/chunks"))
parser.add_argument("--output", type=Path, default=Path("output/combined"))
args = parser.parse_args()

completed = []
for marker in sorted(args.chunks.glob("chunk_*/completion.json")):
    metadata = json.loads(marker.read_text())
    if metadata.get("status") == "complete":
        completed.append(marker.parent)

if not completed:
    raise SystemExit(f"No completed chunks found under {args.chunks}")

args.output.mkdir(parents=True, exist_ok=True)
for filename in ("truth.csv", "hits.csv", "vertices.csv"):
    frames = [pd.read_csv(chunk / filename) for chunk in completed]
    merged = pd.concat(frames, ignore_index=True).sort_values("event_id")
    merged.to_csv(args.output / filename, index=False)

subprocess.run(
    [
        sys.executable,
        str(Path(__file__).with_name("analyze_response.py")),
        "--truth",
        str(args.output / "truth.csv"),
        "--hits",
        str(args.output / "hits.csv"),
        "--vertices",
        str(args.output / "vertices.csv"),
        "--output-dir",
        str(args.output / "analysis"),
    ],
    check=True,
)
print(f"Merged {len(completed)} completed chunks into {args.output}")
