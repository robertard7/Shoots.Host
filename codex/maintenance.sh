#!/usr/bin/env bash
set -euo pipefail

git submodule update --init --recursive || true

export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-/opt/shoots}"
./scripts/build_host.sh
