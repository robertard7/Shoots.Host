#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

./scripts/ci_local.sh

if [ -f .gitmodules ]; then
  echo ".gitmodules is not allowed" >&2
  exit 1
fi
if git submodule status --recursive | grep -q .; then
  echo "submodule entries are not allowed" >&2
  exit 1
fi
if [ -n "${SHOOTS_UPDATE_SNAPSHOTS:-}" ]; then
  echo "SHOOTS_UPDATE_SNAPSHOTS must be unset for release checks" >&2
  exit 1
fi
./scripts/check_legacy_paths.sh
./scripts/package_host.sh
