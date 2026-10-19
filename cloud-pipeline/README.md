# PYTHIA8 → HepMC3 → Geant4 calorimeter pipeline

This project generates minimum-bias proton-proton collisions at
`sqrt(s) = 200 GeV`, transports the stable particles through a simplified
collider detector, and produces event-level calorimeter response plots.

## Detector model

- Aluminum beam pipe at a 4 cm radius.
- Segmented PbWO4 EMCal from 90–110 cm radius.
- Effective iron/scintillator HCal from 120–170 cm radius.
- 64 azimuthal bins and 24 longitudinal bins per calorimeter.
- `FTFP_BERT` Geant4 physics list.
- Headless CSV output suitable for cloud execution.

This geometry is intentionally simplified. It is useful for software studies,
response comparisons, and parameter scans, but it is not an official sPHENIX
detector simulation.

## Run in GitHub Codespaces

1. Open the repository on GitHub.
2. Select **Code → Codespaces → Create codespace on main**.
3. Wait for the development container and Geant4 build to finish.
4. In the Codespaces terminal, run:

```bash
cd cloud-pipeline
EVENTS=100 SEED=12345 bash scripts/run_pipeline.sh
```

Results are written under `cloud-pipeline/output/`.

## Run with GitHub Actions

Open **Actions → Cloud calorimeter simulation → Run workflow**, choose the
number of events and random seed, and start the job. Download the
`calorimeter-results-*` artifact when the job completes.

The initial container build compiles Geant4 and is therefore the slowest run.
Later runs reuse the GitHub Actions build cache.

## Run with Docker

```bash
docker build -t pythia-geant4-cloud -f .devcontainer/Dockerfile .
docker run --rm \
  -e EVENTS=100 -e SEED=12345 \
  -v "$PWD:/workspace" -w /workspace \
  pythia-geant4-cloud \
  bash scripts/run_pipeline.sh
```

## Outputs

- `events.hepmc`: generated HepMC3 events.
- `truth.csv`: stable-particle truth record.
- `hits.csv`: non-zero cell energy deposits.
- `analysis/event_summary.csv`: event-level truth and detector response.
- `analysis/truth_vs_deposit.png`: truth versus deposited energy.
- `analysis/response.png`: calorimeter response distribution.
