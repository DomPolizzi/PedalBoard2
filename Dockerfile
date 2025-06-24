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
  libsoup2.4-dev \
  libjavascriptcoregtk-4.0-dev \
  libsndfile1-dev \
  libxrender-dev \
  libxcomposite-dev \
  libxdamage-dev \
  libxfixes-dev \
  libgtk2.0-0 \
  libgtk2.0-dev \
  libgtk-3-0 \
  libgtk-3-dev \
  libwebkit2gtk-4.0-dev \
  libwebkit2gtk-4.0-37 \
  libglib2.0-dev \
  libpango1.0-dev \
  libcairo2-dev \
  libatk1.0-dev \
  libgdk-pixbuf2.0-dev \
  libssl-dev \
  ca-certificates \
  libgtkmm-3.0-dev \
  ladspa-sdk \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

# Set environment variables to help find GTK and other libraries
ENV PKG_CONFIG_PATH=/usr/lib/pkgconfig:/usr/share/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig
ENV GTK_PATH=/usr/lib/x86_64-linux-gnu/gtk-3.0

# Verify GTK installation
RUN pkg-config --exists gtk+-3.0 && echo "GTK3 found" || echo "GTK3 NOT found"
RUN pkg-config --exists webkit2gtk-4.0 && echo "WebKit2GTK found" || echo "WebKit2GTK NOT found"

# Create user (optional)
RUN useradd -ms /bin/bash juceuser
USER juceuser
WORKDIR /home/juceuser

# Copy project
COPY --chown=juceuser:juceuser JUCE ./JUCE
COPY --chown=juceuser:juceuser ./CMakeLists.txt ./PedalBoard2/
COPY --chown=juceuser:juceuser app/ ./PedalBoard2/app/

WORKDIR /home/juceuser/PedalBoard2/build

# Build with CMake - separate configure and build steps for better debugging
RUN cmake -B . -S .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
RUN cmake --build . --config Debug
