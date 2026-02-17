#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

export SHOOTS_HOST_BIND="${SHOOTS_HOST_BIND:-127.0.0.1}"
export SHOOTS_HOST_PORT="${SHOOTS_HOST_PORT:-8787}"
export SHOOTS_HOST_LOG_LEVEL="${SHOOTS_HOST_LOG_LEVEL:-info}"
export SHOOTS_HOST_LOG_FORMAT="${SHOOTS_HOST_LOG_FORMAT:-text}"
export SHOOTS_HOST_MAX_BODY_BYTES="${SHOOTS_HOST_MAX_BODY_BYTES:-1048576}"
export SHOOTS_HOST_REQ_TIMEOUT_MS="${SHOOTS_HOST_REQ_TIMEOUT_MS:-1000}"
export SHOOTS_HOST_API_KEY="${SHOOTS_HOST_API_KEY:-}"
export SHOOTS_HOST_CORS_ORIGIN="${SHOOTS_HOST_CORS_ORIGIN:-}"
export SHOOTS_HOST_MAX_INFLIGHT="${SHOOTS_HOST_MAX_INFLIGHT:-64}"
export SHOOTS_HOST_SHUTDOWN_DRAIN_MS="${SHOOTS_HOST_SHUTDOWN_DRAIN_MS:-2000}"

echo "Starting ShootsHost"
echo "  bind=$SHOOTS_HOST_BIND"
echo "  port=$SHOOTS_HOST_PORT"
echo "  log_level=$SHOOTS_HOST_LOG_LEVEL"
echo "  log_format=$SHOOTS_HOST_LOG_FORMAT"
echo "  max_body_bytes=$SHOOTS_HOST_MAX_BODY_BYTES"
echo "  req_timeout_ms=$SHOOTS_HOST_REQ_TIMEOUT_MS"
echo "  api_key_enabled=$([ -n "$SHOOTS_HOST_API_KEY" ] && echo true || echo false)"
echo "  cors_origin=$SHOOTS_HOST_CORS_ORIGIN"
echo "  max_inflight=$SHOOTS_HOST_MAX_INFLIGHT"
echo "  shutdown_drain_ms=$SHOOTS_HOST_SHUTDOWN_DRAIN_MS"
echo ""
echo "Endpoints:"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/healthz"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/readyz"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/metrics"
echo "  http://$SHOOTS_HOST_BIND:$SHOOTS_HOST_PORT/status"
echo ""

a="./build/shoots-host"
if [ ! -x "$a" ]; then
  echo "Missing $a. Build first with scripts/build_host.sh or cmake --build build." >&2
  exit 2
fi

exec "$a" \
  --bind "$SHOOTS_HOST_BIND" \
  --port "$SHOOTS_HOST_PORT" \
  --log-level "$SHOOTS_HOST_LOG_LEVEL" \
  --log-format "$SHOOTS_HOST_LOG_FORMAT" \
  --max-body-bytes "$SHOOTS_HOST_MAX_BODY_BYTES" \
  --req-timeout-ms "$SHOOTS_HOST_REQ_TIMEOUT_MS" \
  --api-key "$SHOOTS_HOST_API_KEY" \
  --cors-origin "$SHOOTS_HOST_CORS_ORIGIN" \
  --max-inflight "$SHOOTS_HOST_MAX_INFLIGHT" \
  --shutdown-drain-ms "$SHOOTS_HOST_SHUTDOWN_DRAIN_MS"
