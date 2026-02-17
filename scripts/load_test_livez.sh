#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

BIN="${SHOOTS_HOST_BIN:-./build/shoots-host}"
BIND="${SHOOTS_HOST_BIND:-127.0.0.1}"
PORT="${SHOOTS_HOST_PORT:-18787}"
MAX_INFLIGHT="${SHOOTS_HOST_MAX_INFLIGHT:-1}"
CONCURRENCY="${SHOOTS_LOAD_CONCURRENCY:-64}"
ROUNDS="${SHOOTS_LOAD_ROUNDS:-20}"

if [ ! -x "$BIN" ]; then
  echo "Missing host binary: $BIN" >&2
  echo "Build first: cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/shoots && cmake --build build --config Release" >&2
  exit 2
fi

"$BIN" \
  --bind "$BIND" \
  --port "$PORT" \
  --max-inflight "$MAX_INFLIGHT" \
  --log-level warn \
  --log-format text \
  >/dev/null 2>&1 &
HOST_PID=$!

cleanup() {
  if kill -0 "$HOST_PID" 2>/dev/null; then
    kill "$HOST_PID" 2>/dev/null || true
    wait "$HOST_PID" 2>/dev/null || true
  fi
}
trap cleanup EXIT

for _ in $(seq 1 80); do
  if curl -sS "http://$BIND:$PORT/livez" >/dev/null 2>&1; then
    break
  fi
  sleep 0.05
done

if ! curl -sS "http://$BIND:$PORT/livez" >/dev/null 2>&1; then
  echo "Host did not become ready on http://$BIND:$PORT/livez" >&2
  exit 1
fi

codes="$(
  for _ in $(seq 1 "$ROUNDS"); do
    seq 1 "$CONCURRENCY" | xargs -I{} -P "$CONCURRENCY" curl -s -o /dev/null -w '%{http_code}\n' "http://$BIND:$PORT/livez"
  done
)"

ok_count="$(printf '%s\n' "$codes" | awk '$1==200{c++} END{print c+0}')"
reject_count="$(printf '%s\n' "$codes" | awk '$1==429{c++} END{print c+0}')"
other_count="$(printf '%s\n' "$codes" | awk '$1!=200 && $1!=429{c++} END{print c+0}')"

echo "livez load test summary"
echo "  bind=$BIND"
echo "  port=$PORT"
echo "  max_inflight=$MAX_INFLIGHT"
echo "  concurrency=$CONCURRENCY"
echo "  rounds=$ROUNDS"
echo "  http_200=$ok_count"
echo "  http_429=$reject_count"
echo "  other=$other_count"

if [ "$other_count" -ne 0 ]; then
  echo "Unexpected non-200/non-429 responses detected" >&2
  exit 1
fi

if [ "$reject_count" -eq 0 ]; then
  echo "No 429 responses observed; increase SHOOTS_LOAD_CONCURRENCY/ROUNDS or lower SHOOTS_HOST_MAX_INFLIGHT" >&2
  exit 1
fi

echo "PASS: observed bounded overload behavior with 429 responses"
