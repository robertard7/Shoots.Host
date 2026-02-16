#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

export MISE_NO_ANALYTICS=1
export MISE_LOG_LEVEL="${MISE_LOG_LEVEL:-error}"

REPO_DIR="/workspace/Shoots.Host"
cd "$REPO_DIR"

PREFIX="${CMAKE_PREFIX_PATH:-/opt/shoots}"
mkdir -p "$PREFIX"

# Submodules best-effort
if [ -f .gitmodules ]; then
  git submodule sync --recursive || true
  git -c submodule.external/cpp-httplib.update=none submodule update --init --recursive || true
fi

# If provider isn't installed, do a minimal install (same safe auth approach).
if ! find "$PREFIX" -type f -name 'ShootsProviderConfig.cmake' -print -quit | grep -q .; then
  echo "Shoots.Provider not found under PREFIX=$PREFIX. Installing it (maintenance)..."

  apt-get update -y
  apt-get install -y --no-install-recommends \
    ca-certificates git cmake ninja-build build-essential pkg-config

  PROVIDER_REPO="https://github.com/robertard7/Shoots.Provider.git"
  rm -rf /tmp/Shoots.Provider

  if [ -n "${GITHUB_TOKEN:-}" ]; then
    git -c http.extraHeader="AUTHORIZATION: bearer ${GITHUB_TOKEN}" \
      clone --depth 1 "$PROVIDER_REPO" /tmp/Shoots.Provider
  else
    git clone --depth 1 "$PROVIDER_REPO" /tmp/Shoots.Provider
  fi

  cmake -S /tmp/Shoots.Provider -B /tmp/Shoots.Provider/build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release
  cmake --build /tmp/Shoots.Provider/build
  cmake --install /tmp/Shoots.Provider/build --prefix "$PREFIX"
fi

# Build host (incremental unless CLEAN=1)
if [ "${CLEAN:-0}" = "1" ]; then
  rm -rf build
fi

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PREFIX"

cmake --build build

echo "Maintenance complete. PREFIX=$PREFIX"
