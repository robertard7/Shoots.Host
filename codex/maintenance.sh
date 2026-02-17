#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

export MISE_NO_ANALYTICS=1
export MISE_LOG_LEVEL="${MISE_LOG_LEVEL:-error}"

cd /workspace/Shoots.Host

PREFIX="${CMAKE_PREFIX_PATH:-/opt/shoots}"

# Safety: if provider isn't installed, fail with a clear message.
if [ ! -f "$PREFIX/lib/cmake/ShootsProvider/ShootsProviderConfig.cmake" ]; then
  echo "ShootsProvider not installed under: $PREFIX"
  echo "Run setup script first (it installs Shoots.Provider into the prefix)."
  exit 2
fi

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PREFIX"

cmake --build build
