# Dockerfile for building PIC16F886 firmware with XC8 compiler
# Based on Ubuntu 22.04 for maximum compatibility
FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Enable 32-bit architecture support (required for XC8)
RUN dpkg --add-architecture i386

# Install required dependencies for XC8
RUN apt-get update && apt-get install -y \
    wget \
    unzip \
    make \
    libc6:i386 \
    libstdc++6:i386 \
    libncurses5:i386 \
    && rm -rf /var/lib/apt/lists/*

# Download and install XC8 compiler v2.46
# Using version 2.46 which is stable and supports PIC16F886
ARG XC8_VERSION=2.46
ARG XC8_SHA256=16558f8cf10aa6af8d95adee702e3e57058810cfb44ec75fd4bbde018576092b

# Download XC8 installer with checksum verification
RUN wget --retry-connrefused --waitretry=5 --read-timeout=20 --timeout=15 --tries=5 \
    https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools/xc8-v${XC8_VERSION}-full-install-linux-x64-installer.run \
    -O /tmp/xc8-installer.run && \
    echo "${XC8_SHA256}  /tmp/xc8-installer.run" | sha256sum -c - && \
    chmod +x /tmp/xc8-installer.run && \
    /tmp/xc8-installer.run --mode unattended --netservername localhost && \
    rm /tmp/xc8-installer.run

# Add XC8 to PATH (using explicit version since ARG not available in ENV)
ENV PATH="/opt/microchip/xc8/v2.46/bin:${PATH}"

# Set working directory
WORKDIR /project

# Verify installation
RUN xc8-cc --version

CMD ["/bin/bash"]
