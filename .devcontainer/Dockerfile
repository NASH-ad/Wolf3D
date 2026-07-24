FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc \
    make \
    libcsfml-dev \
    gdb \
    valgrind \
    git \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

USER ubuntu

WORKDIR /workspaces/Wolf3d
