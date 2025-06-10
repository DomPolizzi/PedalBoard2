FROM ubuntu:22.04

# Dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
  build-essential \
  cmake \
  git \
  pkg-config \
  libasound2-dev \
  libx11-dev \
  libxext-dev \
  libxinerama-dev \
  libxrandr-dev \
  libxcursor-dev \
  libfreetype6-dev \
  libcurl4-openssl-dev \
  libglu1-mesa-dev \
  libjack-jackd2-dev \
  libsndfile1-dev \
  libxrender-dev \
  libxcomposite-dev \
  libxdamage-dev \
  libxfixes-dev \
  libgtk-3-dev \
  libssl-dev \
  ca-certificates \
  && rm -rf /var/lib/apt/lists/*

# Create user (optional)
RUN useradd -ms /bin/bash juceuser
USER juceuser
WORKDIR /home/juceuser

# Copy project
COPY --chown=juceuser:juceuser . /home/juceuser/PedalBoard2
WORKDIR /home/juceuser/PedalBoard2/build

# Build with CMake
#RUN cmake -B . -S .. && cmake --build . --config Release
RUN cmake -B . -S .. && cmake --build . --config Debug
