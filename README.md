# Geant4 11.4.2 on macOS

Geant4 11.4.2 was built natively for this Apple Silicon Mac with Qt, OpenGL, GDML, multithreading, and the standard physics datasets.

## Cloud PYTHIA + Geant4 project

The [`cloud-pipeline`](cloud-pipeline/) project is a reproducible collider
simulation that runs without using the local Geant4 installation. It generates
200 GeV proton-proton collisions with PYTHIA 8, writes HepMC3 events, transports
them through a segmented Geant4 calorimeter, and creates CSV summaries and
response plots.

It is configured for both GitHub Codespaces and the manually triggered
**Cloud calorimeter simulation** GitHub Actions workflow. See the
[`cloud-pipeline` guide](cloud-pipeline/README.md) for usage and detector details.

## Location

The complete Geant4 build is located at:

```text
/Users/sijanregmi/Documents/ChatGPT/GEANT4/geant4-v11.4.2-build
```

The physics datasets are located at:

```text
/Users/sijanregmi/Documents/ChatGPT/GEANT4/geant4-v11.4.2-build/data
```

The Geant4 source code is located at:

```text
/Users/sijanregmi/Documents/ChatGPT/GEANT4/geant4-v11.4.2
```

## Load the environment

Open Terminal and run this command before using Geant4:

```zsh
source "/Users/sijanregmi/Documents/ChatGPT/GEANT4/geant4-11.4.2-env.sh"
```

Run this command in every new Terminal session that will use Geant4.

## Run the compiled B1 example

```zsh
source "/Users/sijanregmi/Documents/ChatGPT/GEANT4/geant4-11.4.2-env.sh"
cd "/Users/sijanregmi/Documents/ChatGPT/GEANT4/geant4-B1-test"
./exampleB1 exampleB1.in
```

## Compile and run another Geant4 project

Replace `/path/to/your/project` with the directory containing the project's `CMakeLists.txt`. Replace `your_executable` with the executable name defined by that project.

```zsh
source "/Users/sijanregmi/Documents/ChatGPT/GEANT4/geant4-11.4.2-env.sh"
cd "/path/to/your/project"
cmake -S . -B build -DGeant4_DIR="$GEANT4_DIR"
cmake --build build --parallel 8
./build/your_executable
```

## Notes

- Keep `geant4-v11.4.2-build` and its `data` directory in their current locations. The environment script uses these paths.
- `geant4-B1-test` is the compiled verification example.
- If macOS or the external drive becomes unresponsive because of earlier filesystem operations, restart the Mac before running Geant4.

## Repository contents

The downloaded Geant4 source, compiled build, datasets, and compiled B1 example are intentionally excluded from this repository because they are generated or upstream artifacts. Follow the official Geant4 installation instructions to recreate them, then update the local paths above if needed.
