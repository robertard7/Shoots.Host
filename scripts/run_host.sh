#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

export SHOOTS_HOST_BIND="${SHOOTS_HOST_BIND:-127.0.0.1}"
export SHOOTS_HOST_PORT="${SHOOTS_HOST_PORT:-8787}"
export SHOOTS_HOST_LOG_LEVEL="${SHOOTS_HOST_LOG_LEVEL:-info}"
export SHOOTS_HOST_MAX_BODY_BYTES="${SHOOTS_HOST_MAX_BODY_BYTES:-1048576}"
export SHOOTS_HOST_REQ_TIMEOUT_MS="${SHOOTS_HOST_REQ_TIMEOUT_MS:-1000}"

echo "Starting ShootsHost"
echo "  bind=$SHOOTS_HOST_BIND"
echo "  port=$SHOOTS_HOST_PORT"
echo "  log_level=$SHOOTS_HOST_LOG_LEVEL"
echo "  max_body_bytes=$SHOOTS_HOST_MAX_BODY_BYTES"
echo "  req_timeout_ms=$SHOOTS_HOST_REQ_TIMEOUT_MS"
echo ""
echo "Endpoints:"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/healthz"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/readyz"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/metrics"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/status"
echo ""
echo "Try:"
echo "  curl -s http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/healthz"
echo "  curl -s http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/readyz"
echo "  curl -s http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/metrics"

a="./build/ShootsHost"
if [ ! -x "$a" ]; then
  echo "Missing $a. Build first with scripts/build_host.sh or cmake --build build." >&2
  exit 2
fi

exec "$a" \
  --bind "$SHOOTS_HOST_BIND" \
  --port "$SHOOTS_HOST_PORT" \
  --log-level "$SHOOTS_HOST_LOG_LEVEL" \
  --max-body-bytes "$SHOOTS_HOST_MAX_BODY_BYTES" \
  --req-timeout-ms "$SHOOTS_HOST_REQ_TIMEOUT_MS"
