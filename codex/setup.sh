#!/usr/bin/env bash
set -euo pipefail

export DEBIAN_FRONTEND=noninteractive
export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

# keep mise from being noisy/pointless
export MISE_NO_ANALYTICS=1
export MISE_LOG_LEVEL="${MISE_LOG_LEVEL:-error}"

ROOT="$(pwd)"
PREFIX="${CMAKE_PREFIX_PATH:-/opt/shoots}"

apt-get update -y
apt-get install -y --no-install-recommends \
  ca-certificates git cmake ninja-build build-essential pkg-config

mkdir -p "$PREFIX"

# ---- Install Shoots.Provider into PREFIX ----
# If the repo is private, Codex must provide GITHUB_TOKEN with repo read access.
PROVIDER_DIR="/tmp/Shoots.Provider"
PROVIDER_URL="https://github.com/robertard7/Shoots.Provider.git"
if [[ -n "${GITHUB_TOKEN:-}" ]]; then
  PROVIDER_URL="https://${GITHUB_TOKEN}@github.com/robertard7/Shoots.Provider.git"
fi

rm -rf "$PROVIDER_DIR"
git clone --depth 1 "$PROVIDER_URL" "$PROVIDER_DIR"

cmake -S "$PROVIDER_DIR" -B "$PROVIDER_DIR/build" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release

cmake --build "$PROVIDER_DIR/build"
cmake --install "$PROVIDER_DIR/build" --prefix "$PREFIX"

# Verify install produced the config package where Host expects it
test -f "$PREFIX/lib/cmake/ShootsProvider/ShootsProviderConfig.cmake"

# ---- Now build Shoots.Host against installed Provider ----
cd "$ROOT"

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PREFIX"

cmake --build build
