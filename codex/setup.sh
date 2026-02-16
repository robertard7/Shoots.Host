#!/usr/bin/env bash
set -euo pipefail

export DEBIAN_FRONTEND=noninteractive
export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

# keep mise from being noisy/pointless
export MISE_NO_ANALYTICS=1
export MISE_LOG_LEVEL="${MISE_LOG_LEVEL:-error}"

# Codex clones here. Do not trust $PWD.
REPO_DIR="/workspace/Shoots.Host"
cd "$REPO_DIR"

PREFIX="${CMAKE_PREFIX_PATH:-/opt/shoots}"
mkdir -p "$PREFIX"

# ---- Base build deps ----
apt-get update -y
apt-get install -y --no-install-recommends \
  ca-certificates git cmake ninja-build build-essential pkg-config

# ---- Submodules (best-effort; do not block build) ----
if [ -f .gitmodules ]; then
  git submodule sync --recursive || true
  # Prevent update failures if a submodule entry exists but lacks a url / isn’t needed.
  git -c submodule.external/cpp-httplib.update=none submodule update --init --recursive || true
fi

# ---- Install Shoots.Provider into PREFIX (only if missing) ----
# We detect installation by presence of ShootsProviderConfig.cmake somewhere under PREFIX.
if ! find "$PREFIX" -type f -name 'ShootsProviderConfig.cmake' -print -quit | grep -q .; then
  echo "Shoots.Provider not found under PREFIX=$PREFIX. Installing it..."

  PROVIDER_REPO="https://github.com/robertard7/Shoots.Provider.git"
  rm -rf /tmp/Shoots.Provider

  if [ -n "${GITHUB_TOKEN:-}" ]; then
    # Use header-based auth (safer than embedding token in URL).
    git -c http.extraHeader="AUTHORIZATION: bearer ${GITHUB_TOKEN}" \
      clone --depth 1 "$PROVIDER_REPO" /tmp/Shoots.Provider
  else
    git clone --depth 1 "$PROVIDER_REPO" /tmp/Shoots.Provider
  fi

  cmake -S /tmp/Shoots.Provider -B /tmp/Shoots.Provider/build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release

  cmake --build /tmp/Shoots.Provider/build
  cmake --install /tmp/Shoots.Provider/build --prefix "$PREFIX"
else
  echo "Shoots.Provider already installed under PREFIX=$PREFIX. Skipping provider install."
fi

# ---- Build Shoots.Host ----
rm -rf build
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PREFIX"

cmake --build build

# ---- Sanity: prove provider config is actually visible ----
if ! find "$PREFIX" -type f -name 'ShootsProviderConfig.cmake' -print -quit | grep -q .; then
  echo "ERROR: ShootsProviderConfig.cmake still not present under PREFIX=$PREFIX after install."
  echo "Check Shoots.Provider install step and CMake install rules."
  exit 1
fi

echo "Setup complete. PREFIX=$PREFIX"
