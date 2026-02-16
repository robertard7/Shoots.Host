#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

export MISE_NO_ANALYTICS=1
export MISE_LOG_LEVEL="${MISE_LOG_LEVEL:-error}"

cd /workspace/Shoots.Host

PREFIX="${CMAKE_PREFIX_PATH:-/opt/shoots}"

git submodule sync --recursive || true
git -c submodule.external/cpp-httplib.update=none submodule update --init --recursive || true

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PREFIX"

cmake --build build
