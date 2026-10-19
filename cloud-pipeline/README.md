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

From a Mac Terminal, the convenience script starts chunk `0` with 20 events by
default:

```bash
cd cloud-pipeline
./scripts/run_cloud_chunk.sh
```

Pass a chunk index and events per chunk when needed:

```bash
./scripts/run_cloud_chunk.sh 1 20
```

An optional third parameter changes the base seed; normally leave its default
value of `12345` unchanged.

After submission, monitor the workflow until it succeeds or fails:

```bash
gh run watch --repo regmi-sijan/geant4_sr --exit-status
```

A successful run displays a green check on GitHub and makes its
`calorimeter-chunk-*` result artifact available for download.

During execution, expand **Run PYTHIA and Geant4** on the Actions page to see
per-event messages such as `Starting event 7/20` and `Completed event 7/20`.
The complete terminal output is also saved as `run.log` inside the downloaded
chunk artifact, so it remains available after the cloud runner is destroyed.

Open **Actions → Cloud calorimeter simulation → Run workflow**. Choose a chunk
number, events per chunk, and base seed. Start with chunk `0`, then run chunks
`1`, `2`, and so on. Each successful chunk is uploaded independently as a
`calorimeter-chunk-*` artifact and retained for 30 days.

If a job is cancelled or reaches a usage limit, rerun the same chunk number.
Its seed and global event-number range are deterministic, so it safely replaces
the interrupted attempt. Completed chunks never need to be simulated again.
GitHub also caches the compiled Geant4 container, independently of result
artifacts.

After downloading and placing chunk directories under `output/chunks/`, merge
all chunks that contain a `completion.json` marker:

```bash
python analysis/merge_chunks.py
```

The marker is written only after generation, transport, and analysis all
succeed, preventing partial chunks from being mistaken for valid results.

The initial container build compiles Geant4 and is therefore the slowest run.
Later runs reuse the GitHub Actions build cache. For free-tier use, 10–50 events
per chunk is a practical starting range; increase it only after observing the
runtime of your selected physics configuration.

## Run with Docker

```bash
docker build -t pythia-geant4-cloud -f .devcontainer/Dockerfile .
docker run --rm \
  -e EVENTS=25 -e CHUNK_INDEX=0 -e EVENT_OFFSET=0 -e SEED=12345 \
  -v "$PWD:/workspace" -w /workspace \
  pythia-geant4-cloud \
  bash scripts/run_pipeline.sh
```

## Outputs

- `events.hepmc`: generated HepMC3 events.
- `truth.csv`: stable-particle truth record.
- `hits.csv`: non-zero cell energy deposits.
- `run.log`: persistent build, generation, transport, and analysis log.
- `analysis/event_summary.csv`: event-level truth and detector response.
- `analysis/truth_vs_deposit.png`: truth versus deposited energy.
- `analysis/response.png`: calorimeter response distribution.
