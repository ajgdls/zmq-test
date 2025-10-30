# Shared setup stage ###########################################################
FROM ubuntu:24.04 AS common

ENV PATH=/odin/bin:/venv/bin:$PATH

# Get fundamental packages
RUN apt-get update -y && \
    apt-get install --no-install-recommends -y curl \
    tar ca-certificates software-properties-common && \
    apt-get -y clean all

# Developer stage for devcontainer #############################################
FROM common AS developer

# System dependencies
RUN add-apt-repository -y ppa:deadsnakes/ppa && \
    apt-get update -y && apt-get install -y --no-install-recommends \
    # General build
    build-essential cmake git \
    # odin-data C++ dependencies
    libblosc-dev libboost-all-dev libhdf5-dev liblog4cxx-dev libpcap-dev libczmq-dev \
    # python
    python3.11-dev python3.11-venv && \
    # tidy up
    apt-get -y clean all

# Build stage - throwaway stage for runtime assets #############################
FROM developer AS build

# Copy in project
WORKDIR /test_zmq
COPY . .

# C++
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/test_zmq .. && \
    make -j8 VERBOSE=1 && \
    make install

