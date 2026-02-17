#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

matches="$(rg -n "external/cpp-httplib|cpp-httplib" . \
  -g '!external/httplib/**' \
  -g '!THIRD_PARTY_NOTICES.md' \
  -g '!scripts/check_legacy_paths.sh' \
  -g '!.git/**' || true)"

if [[ -n "$matches" ]]; then
  echo "Found legacy cpp-httplib references outside vendored/license files:"
  echo "$matches"
  exit 1
fi

echo "Legacy path check passed"
