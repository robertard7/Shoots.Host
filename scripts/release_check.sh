#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

./scripts/ci_local.sh
./scripts/check_legacy_paths.sh
./scripts/package_host.sh
