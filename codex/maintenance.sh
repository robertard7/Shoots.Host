#!/usr/bin/env bash
set -euo pipefail

export DEBIAN_FRONTEND=noninteractive
export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

export MISE_NO_ANALYTICS=1
export MISE_LOG_LEVEL="${MISE_LOG_LEVEL:-error}"
export MISE_DEBUG="${MISE_DEBUG:-0}"

export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-/opt/shoots}"

log() { printf '[maint] %s\n' "$*"; }

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"
log "repo: $REPO_ROOT"
log "CMAKE_PREFIX_PATH: $CMAKE_PREFIX_PATH"

git config --global advice.detachedHead false || true

# Keep submodules aligned; never block if GitHub hiccups.
git submodule sync --recursive || true
git -c submodule.external/cpp-httplib.update=none submodule update --init --recursive || true

# Ensure helper scripts can run
if [ -d "./scripts" ]; then
  chmod +x ./scripts/*.sh 2>/dev/null || true
fi

# Optional: refresh tools via mise if present (non-fatal).
if command -v mise >/dev/null 2>&1; then
  if [ -f ".mise.toml" ] || [ -f "mise.toml" ] || [ -f ".tool-versions" ]; then
    log "mise install (best-effort)"
    mise install || true
  fi
fi

# Incremental build if possible, else configure+build.
if [ -d "build" ] && [ -f "build/build.ninja" ]; then
  log "incremental build"
  cmake --build build
else
  log "configure + build"
  cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH"
  cmake --build build
fi

log "maintenance complete"
