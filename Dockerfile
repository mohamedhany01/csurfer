# Use a stable Ubuntu base
FROM ubuntu:noble

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Build-time arguments for user ID and group ID (matched to host)
ARG USER_ID=1000
ARG GROUP_ID=1000

# 1. Install all dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    clang \
    clang-tidy \
    clang-format \
    libssl-dev \
    libsdl2-dev \
    libsdl2-ttf-dev \
    libfreetype-dev \
    mesa-utils \
    libgl1-mesa-dri \
    git \
    pkg-config \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# 2. Create a non-root user matching the host UID/GID
# We remove any existing user with the same ID (common in recent Ubuntu images)
RUN if getent passwd ${USER_ID}; then userdel -f $(getent passwd ${USER_ID} | cut -d: -f1); fi && \
    if getent group ${GROUP_ID}; then groupdel $(getent group ${GROUP_ID} | cut -d: -f1); fi && \
    groupadd -g ${GROUP_ID} developer && \
    useradd -u ${USER_ID} -g developer -m -s /bin/bash developer && \
    echo "developer ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# 3. Set up working directory
WORKDIR /home/developer/csurfer
USER developer

# Default command (just keeps container alive or ready for build)
CMD ["/bin/bash"]
