"""Write a completion marker only after every pipeline stage succeeds."""

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path


parser = argparse.ArgumentParser()
parser.add_argument("--output", type=Path, required=True)
parser.add_argument("--chunk-index", type=int, required=True)
parser.add_argument("--event-offset", type=int, required=True)
parser.add_argument("--events", type=int, required=True)
parser.add_argument("--seed", type=int, required=True)
parser.add_argument("--vertex-z-sigma-mm", type=float, required=True)
parser.add_argument("--vertex-z-max-mm", type=float, required=True)
args = parser.parse_args()

payload = {
    "status": "complete",
    "chunk_index": args.chunk_index,
    "event_offset": args.event_offset,
    "events": args.events,
    "seed": args.seed,
    "vertex_z_sigma_mm": args.vertex_z_sigma_mm,
    "vertex_z_max_mm": args.vertex_z_max_mm,
    "completed_at": datetime.now(timezone.utc).isoformat(),
}
args.output.write_text(json.dumps(payload, indent=2) + "\n")
