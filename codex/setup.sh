#!/usr/bin/env bash
set -euo pipefail

export MISE_DISABLE=1
export MISE_ENV=disable
export MISE_LOG_LEVEL=error
export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-/opt/shoots}"

./scripts/setup_provider_prefix.sh
./scripts/build_host.sh
