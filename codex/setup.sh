#!/usr/bin/env bash
set -euo pipefail

export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-/opt/shoots}"

./scripts/setup_provider_prefix.sh
./scripts/build_host.sh
