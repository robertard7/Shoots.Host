#!/usr/bin/env bash
set -euo pipefail

# -----------------------------
# Codex non-interactive guardrails
# -----------------------------
export DEBIAN_FRONTEND=noninteractive
export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

# Keep mise quiet if present.
export MISE_NO_ANALYTICS=1
export MISE_LOG_LEVEL="${MISE_LOG_LEVEL:-error}"
export MISE_DEBUG="${MISE_DEBUG:-0}"

# Default prefix for provider installs / find_package.
export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-/opt/shoots}"

log() { printf '[setup] %s\n' "$*"; }

# Repo root (Codex clones to /workspace/<repo>)
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"
log "repo: $REPO_ROOT"
log "CMAKE_PREFIX_PATH: $CMAKE_PREFIX_PATH"

# Prefix exists + writable (no sudo assumptions)
mkdir -p "$CMAKE_PREFIX_PATH" || true
if [ ! -w "$CMAKE_PREFIX_PATH" ]; then
  log "prefix not writable; attempting chown (best-effort)"
  chown -R "$(id -u)":"$(id -g)" "$CMAKE_PREFIX_PATH" 2>/dev/null || true
fi

# Submodules (never block, never prompt)
git config --global advice.detachedHead false || true
git submodule sync --recursive || true
git -c submodule.external/cpp-httplib.update=none submodule update --init --recursive || true

# Ensure helper scripts can run
if [ -d "./scripts" ]; then
  chmod +x ./scripts/*.sh 2>/dev/null || true
fi

# Toolchain sanity (best-effort). Install only if missing.
need_pkg=0
for bin in cmake ninja git pkg-config; do
  command -v "$bin" >/dev/null 2>&1 || need_pkg=1
done

if command -v apt-get >/dev/null 2>&1 && [ "$need_pkg" -eq 1 ]; then
  log "installing missing build tools (best-effort)"
  apt-get update -y || true
  apt-get install -y --no-install-recommends \
    ca-certificates git cmake ninja-build build-essential pkg-config \
    || true
fi

# mise: only if present AND repo declares tools
if command -v mise >/dev/null 2>&1; then
  if [ -f ".mise.toml" ] || [ -f "mise.toml" ] || [ -f ".tool-versions" ]; then
    log "mise detected + tool file present: installing declared tools"
    mise install || true
    mise exec -- node -v || true
    mise exec -- go version || true
    mise exec -- ruby -v || true
  else
    log "mise detected but no tool file found; skipping mise install"
  fi
else
  log "mise not found; skipping mise steps"
fi

run_cmd() {
  local seconds="$1"; shift
  local cmd="$*"
  if command -v timeout >/dev/null 2>&1; then
    timeout "${seconds}" bash -lc "${cmd}"
  else
    bash -lc "${cmd}"
  fi
}

# Provider prefix step (preferred)
if [ -x "./scripts/setup_provider_prefix.sh" ]; then
  log "running ./scripts/setup_provider_prefix.sh"
  run_cmd 600 "./scripts/setup_provider_prefix.sh"
else
  log "no ./scripts/setup_provider_prefix.sh found; skipping provider prefix step"
fi

# Build host (preferred)
if [ -x "./scripts/build_host.sh" ]; then
  log "running ./scripts/build_host.sh"
  run_cmd 600 "./scripts/build_host.sh"
else
  log "no ./scripts/build_host.sh found; doing direct cmake build"
  cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH"
  cmake --build build
fi

log "setup complete"
