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
- A shared collision vertex per event, read by Geant4 from `vertices.csv`, with
  `x = y = 0` and Gaussian
  `z` (`sigma = 33.33 cm`), truncated at `-100 cm < z < 100 cm`.
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

### Complete run, monitoring, and download checklist

The wrapper submits the calculation to GitHub Actions. The simulation runs on
GitHub's cloud runner, not in the Mac Terminal process, so the result is **not
automatically copied to the laptop**.

1. From the local project directory, submit a run. With no arguments this uses
   chunk `0`, 20 events, and base seed `12345`:

   ```bash
   cd /Users/sijanregmi/Documents/ChatGPT/GEANT4/cloud-pipeline
   ./scripts/run_cloud_chunk.sh
   ```

   To select the chunk and number of events explicitly:

   ```bash
   ./scripts/run_cloud_chunk.sh 1 20
   ```

   An optional third argument changes the base seed; normally leave its
   default value of `12345` unchanged.

2. List recent workflow runs and note the numeric run ID of the submitted run:

   ```bash
   gh run list --repo regmi-sijan/geant4_sr --workflow cloud-simulation.yml
   ```

3. Watch that run's workflow-step progress in the Mac Terminal, replacing
   `RUN_ID` with its number:

   ```bash
   gh run watch RUN_ID --repo regmi-sijan/geant4_sr --exit-status
   ```

   The command exits successfully when the workflow completes, or returns an
   error if it fails. Closing this terminal does not stop a submitted cloud
   job and does not erase its cloud output.

4. For detailed live output, open the workflow on the
   [GitHub Actions page](https://github.com/regmi-sijan/geant4_sr/actions) and
   expand **Run PYTHIA and Geant4**. It displays pipeline phases and Geant4
   event messages such as `Starting event 7/20` and `Completed event 7/20`.

5. Download that run's artifact to the laptop, replacing `RUN_ID` with the
   number from the previous command:

   ```bash
   ./scripts/download_cloud_results.sh RUN_ID
   ```

   To automatically download the latest successful cloud simulation, omit the
   run ID:

   ```bash
   ./scripts/download_cloud_results.sh
   ```

   The equivalent direct GitHub CLI command is:

   ```bash
   gh run download RUN_ID --repo regmi-sijan/geant4_sr \
     --pattern 'calorimeter-chunk-*' \
     --dir ./geant4-results/run-RUN_ID
   ```

   The downloaded files are then kept locally under:

   ```text
   /Users/sijanregmi/Documents/ChatGPT/GEANT4/cloud-pipeline/geant4-results/run-RUN_ID/
   ```

6. Find and read the persistent pipeline log:

   ```bash
   find ./geant4-results/run-RUN_ID -name run.log
   less ./geant4-results/run-RUN_ID/calorimeter-chunk-*/chunk_*/run.log
   ```

   Press `q` to exit `less`. GitHub's copy of the same output can be viewed
   without downloading the artifact:

   ```bash
   gh run view RUN_ID --repo regmi-sijan/geant4_sr --log
   ```

7. Keep any important results on the laptop or an external drive. GitHub
   retains this workflow's artifacts for 30 days; local downloaded copies do
   not expire automatically.

Each downloaded chunk contains:

```text
chunk_XXXXX/
├── run.log
├── events.hepmc
├── truth.csv
├── vertices.csv
├── hits.csv
├── completion.json
└── analysis/
    ├── event_summary.csv
    ├── response.png
    └── truth_vs_deposit.png
```

`completion.json` confirms that event generation, Geant4 transport, and
analysis all finished. If it is absent, treat that chunk as incomplete and
rerun the same chunk index.

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
- `vertices.csv`: one shared collision vertex per event, in millimetres.
- `hits.csv`: non-zero cell energy deposits.
- `run.log`: persistent build, generation, transport, and analysis log.
- `analysis/event_summary.csv`: one row for every generated event, retaining
  its collision vertex coordinates alongside truth and detector response.
- `analysis/truth_vs_deposit.png`: truth versus deposited energy.
- `analysis/response.png`: calorimeter response distribution.
