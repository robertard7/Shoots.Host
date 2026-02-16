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
# If the repo is private, Codex must have a GITHUB_TOKEN secret with repo read access.
# We'll use it if present.
PROVIDER_URL="https://github.com/robertard7/Shoots.Provider.git"
if [[ -n "${GITHUB_TOKEN:-}" ]]; then
  PROVIDER_URL="https://${GITHUB_TOKEN}@github.com/robertard7/Shoots.Provider.git"
fi

rm -rf /tmp/Shoots.Provider
git clone --depth 1 "$PROVIDER_URL" /tmp/Shoots.Provider

cmake -S /tmp/Shoots.Provider -B /tmp/Shoots.Provider/build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release

cmake --build /tmp/Shoots.Provider/build
cmake --install /tmp/Shoots.Provider/build --prefix "$PREFIX"

# ---- Now build Shoots.Host against installed Provider ----
cd "$ROOT"

git submodule sync --recursive || true
git -c submodule.external/cpp-httplib.update=none submodule update --init --recursive || true

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PREFIX"

cmake --build build
