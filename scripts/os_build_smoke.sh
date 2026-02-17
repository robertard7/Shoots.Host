#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

BIN="${SHOOTS_HOST_BIN:-./build/shoots-host}"
BIND="${SHOOTS_HOST_BIND:-127.0.0.1}"
PORT="${SHOOTS_HOST_PORT:-18788}"
API_KEY="${SHOOTS_HOST_API_KEY:-}"

if [ ! -x "$BIN" ]; then
  echo "Missing host binary: $BIN" >&2
  echo "Build first: cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/shoots && cmake --build build --config Release" >&2
  exit 2
fi

start_args=(
  --bind "$BIND"
  --port "$PORT"
  --log-level warn
  --log-format text
)

if [ -n "$API_KEY" ]; then
  start_args+=(--api-key "$API_KEY")
fi

"$BIN" "${start_args[@]}" >/dev/null 2>&1 &
HOST_PID=$!

cleanup() {
  if kill -0 "$HOST_PID" 2>/dev/null; then
    kill "$HOST_PID" 2>/dev/null || true
    wait "$HOST_PID" 2>/dev/null || true
  fi
}
trap cleanup EXIT

for _ in $(seq 1 100); do
  if curl -fsS "http://$BIND:$PORT/livez" >/dev/null 2>&1; then
    break
  fi
  sleep 0.05
done

curl -fsS "http://$BIND:$PORT/livez" >/dev/null
curl -fsS "http://$BIND:$PORT/healthz" >/dev/null

auth_header=()
if [ -n "$API_KEY" ]; then
  auth_header=(-H "X-Api-Key: $API_KEY")
fi

status_body="$(curl -fsS "${auth_header[@]}" "http://$BIND:$PORT/status")"
build_body='{"language":"cpp","payload":{"entry":"main"},"templateId":"build/default"}'
submit_body="$(curl -fsS "${auth_header[@]}" -H 'Content-Type: application/json' -d "$build_body" "http://$BIND:$PORT/v1/build")"

python - <<'PY' "$status_body" "$submit_body"
import json
import sys

status = json.loads(sys.argv[1])
submit = json.loads(sys.argv[2])

if not status.get('ok'):
    raise SystemExit('status endpoint did not return ok=true')
if not status.get('result', {}).get('ready', False):
    raise SystemExit('status endpoint reported ready=false')
if not submit.get('ok'):
    raise SystemExit('build submit did not return ok=true')
if 'jobId' not in submit.get('result', {}):
    raise SystemExit('build submit missing result.jobId')
if 'requestId' not in submit.get('result', {}):
    raise SystemExit('build submit missing result.requestId')
PY

echo "PASS: os build smoke checks succeeded"
