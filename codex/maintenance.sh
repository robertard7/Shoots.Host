#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true
export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-/opt/shoots}"

git config --global advice.detachedHead false || true
git -c submodule.external/cpp-httplib.update=none submodule update --init --recursive || true

if command -v timeout >/dev/null 2>&1; then
  timeout 300 ./scripts/build_host.sh
else
  ./scripts/build_host.sh
fi
