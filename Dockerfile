# Dockerfile for building PIC16F886 firmware with XC8 compiler
# Based on Ubuntu 22.04 for maximum compatibility
FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required dependencies for XC8
RUN apt-get update && apt-get install -y \
    wget \
    unzip \
    make \
    libc6:i386 \
    libstdc++6:i386 \
    libncurses5:i386 \
    && rm -rf /var/lib/apt/lists/*

# Enable 32-bit architecture support (required for XC8)
RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y libc6:i386 libstdc++6:i386 && \
    rm -rf /var/lib/apt/lists/*

# Download and install XC8 compiler v2.46
# Using version 2.46 which is stable and supports PIC16F886
ARG XC8_VERSION=2.46
RUN wget --quiet --retry-connrefused --tries=5 --timeout=30 \
    https://ww1.microchip.com/downloads/en/DeviceDoc/xc8-v${XC8_VERSION}-full-install-linux-x64-installer.run \
    -O /tmp/xc8-installer.run && \
    chmod +x /tmp/xc8-installer.run && \
    /tmp/xc8-installer.run --mode unattended --netservername localhost && \
    rm /tmp/xc8-installer.run

# Add XC8 to PATH
ENV PATH="/opt/microchip/xc8/v${XC8_VERSION}/bin:${PATH}"

# Set working directory
WORKDIR /project

# Verify installation
RUN xc8-cc --version

CMD ["/bin/bash"]
