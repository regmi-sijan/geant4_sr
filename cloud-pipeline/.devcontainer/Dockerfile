FROM ubuntu:24.04

ARG GEANT4_VERSION=11.4.2
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential ca-certificates cmake curl git libexpat1-dev libhepmc3-dev \
    libxerces-c-dev ninja-build python3 python3-dev python3-pip python3-venv \
    zlib1g-dev && \
    rm -rf /var/lib/apt/lists/*

RUN python3 -m venv /opt/venv
ENV PATH=/opt/venv/bin:$PATH
COPY requirements.txt /tmp/requirements.txt
RUN pip install --no-cache-dir -r /tmp/requirements.txt

RUN curl -fsSL "https://github.com/Geant4/geant4/archive/refs/tags/v${GEANT4_VERSION}.tar.gz" \
      -o /tmp/geant4.tar.gz && \
    tar -xzf /tmp/geant4.tar.gz -C /tmp && \
    cmake -S "/tmp/geant4-${GEANT4_VERSION}" -B /tmp/geant4-build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/opt/geant4 \
      -DGEANT4_BUILD_MULTITHREADED=ON \
      -DGEANT4_INSTALL_DATA=ON \
      -DGEANT4_USE_GDML=ON \
      -DGEANT4_USE_SYSTEM_EXPAT=ON \
      -DGEANT4_USE_SYSTEM_ZLIB=ON && \
    cmake --build /tmp/geant4-build --parallel "$(nproc)" && \
    cmake --install /tmp/geant4-build && \
    rm -rf /tmp/geant4.tar.gz /tmp/geant4-build "/tmp/geant4-${GEANT4_VERSION}"

ENV PATH=/opt/geant4/bin:$PATH
ENV CMAKE_PREFIX_PATH=/opt/geant4
WORKDIR /workspace
CMD ["bash"]
