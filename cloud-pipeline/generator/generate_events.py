"""Generate RHIC p+p events for the Geant4 calorimeter pipeline."""

import argparse
import csv
import os
import random
from pathlib import Path

os.environ.pop("PYTHIA8DATA", None)

import pyhepmc
import pythia8mc


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--events", type=int, default=100)
    parser.add_argument("--seed", type=int, default=12345)
    parser.add_argument("--event-offset", type=int, default=0)
    parser.add_argument("--vertex-z-sigma-mm", type=float, default=1000.0 / 3.0)
    parser.add_argument("--vertex-z-max-mm", type=float, default=1000.0)
    parser.add_argument("--hepmc", type=Path, default=Path("output/events.hepmc"))
    parser.add_argument("--truth", type=Path, default=Path("output/truth.csv"))
    parser.add_argument("--vertices", type=Path, default=Path("output/vertices.csv"))
    return parser.parse_args()


def configure(seed: int) -> pythia8mc.Pythia:
    pythia = pythia8mc.Pythia()
    settings = [
        "Beams:idA = 2212",
        "Beams:idB = 2212",
        "Beams:eCM = 200.",
        "SoftQCD:inelastic = on",
        "MultipartonInteractions:Kfactor = 0.5",
        "PhaseSpace:pTHatMin = 0.5",
        "Next:numberShowInfo = 0",
        "Next:numberShowProcess = 0",
        "Next:numberShowEvent = 0",
        "Random:setSeed = on",
        f"Random:seed = {seed}",
    ]
    for setting in settings:
        if not pythia.readString(setting):
            raise RuntimeError(f"PYTHIA rejected setting: {setting}")
    if not pythia.init():
        raise RuntimeError("PYTHIA initialization failed")
    return pythia


def main() -> None:
    args = parse_args()
    if args.events < 1:
        raise ValueError("--events must be positive")
    if not 1 <= args.seed <= 900_000_000:
        raise ValueError("--seed must be between 1 and 900000000")
    if args.event_offset < 0:
        raise ValueError("--event-offset must be non-negative")
    if args.vertex_z_sigma_mm <= 0:
        raise ValueError("--vertex-z-sigma-mm must be positive")
    if args.vertex_z_max_mm <= 0:
        raise ValueError("--vertex-z-max-mm must be positive")

    args.hepmc.parent.mkdir(parents=True, exist_ok=True)
    args.truth.parent.mkdir(parents=True, exist_ok=True)
    args.vertices.parent.mkdir(parents=True, exist_ok=True)
    pythia = configure(args.seed)
    vertex_rng = random.Random(args.seed)
    with pyhepmc.io.WriterAscii(str(args.hepmc)) as writer, args.truth.open(
        "w", newline=""
    ) as truth_file, args.vertices.open("w", newline="") as vertex_file:
        truth = csv.writer(truth_file)
        truth.writerow(["event_id", "particle_id", "pdg", "px_gev", "py_gev", "pz_gev", "energy_gev"])
        vertices = csv.writer(vertex_file)
        vertices.writerow(["event_id", "vertex_x_mm", "vertex_y_mm", "vertex_z_mm"])

        generated = 0
        while generated < args.events:
            if not pythia.next():
                continue

            # Use one shared beam-collision vertex for every final-state particle
            # in the event. Draw z from a Gaussian and reject the tails outside
            # the configured three-sigma envelope. The separate seeded RNG keeps
            # the vertex reproducible without changing PYTHIA's random sequence.
            vertex_z_mm = vertex_rng.gauss(0.0, args.vertex_z_sigma_mm)
            while abs(vertex_z_mm) > args.vertex_z_max_mm:
                vertex_z_mm = vertex_rng.gauss(0.0, args.vertex_z_sigma_mm)

            event = pyhepmc.GenEvent(pyhepmc.Units.GEV, pyhepmc.Units.MM)
            event_id = args.event_offset + generated
            event.event_number = event_id
            vertices.writerow([event_id, 0.0, 0.0, vertex_z_mm])

            particle_id = 0
            for particle in pythia.event:
                if not particle.isFinal():
                    continue
                event.add_particle(
                    pyhepmc.GenParticle(
                        (particle.px(), particle.py(), particle.pz(), particle.e()),
                        particle.id(),
                        1,
                    )
                )
                truth.writerow(
                    [
                        event_id,
                        particle_id,
                        particle.id(),
                        particle.px(),
                        particle.py(),
                        particle.pz(),
                        particle.e(),
                    ]
                )
                particle_id += 1
            writer.write_event(event)
            generated += 1
            print(
                f"generated event {generated}/{args.events} "
                f"at vertex z={vertex_z_mm:.3f} mm"
            )

    pythia.stat()
    print(f"wrote {args.hepmc}, {args.truth}, and {args.vertices}")


if __name__ == "__main__":
    main()
