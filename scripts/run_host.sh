#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

export SHOOTS_HOST_BIND="${SHOOTS_HOST_BIND:-127.0.0.1}"
export SHOOTS_HOST_PORT="${SHOOTS_HOST_PORT:-8787}"
export SHOOTS_HOST_LOG_LEVEL="${SHOOTS_HOST_LOG_LEVEL:-info}"

echo "Starting ShootsHost"
echo "  bind=$SHOOTS_HOST_BIND"
echo "  port=$SHOOTS_HOST_PORT"
echo "  log_level=$SHOOTS_HOST_LOG_LEVEL"
echo ""
echo "Endpoints:"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/healthz"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/readyz"
echo ""
echo "Try:"
echo "  curl -s http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/healthz"
echo "  curl -s http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/readyz"

a="./build/ShootsHost"
if [ ! -x "$a" ]; then
  echo "Missing $a. Build first with scripts/build_host.sh or cmake --build build." >&2
  exit 2
fi

exec "$a" --bind "$SHOOTS_HOST_BIND" --port "$SHOOTS_HOST_PORT" --log-level "$SHOOTS_HOST_LOG_LEVEL"
