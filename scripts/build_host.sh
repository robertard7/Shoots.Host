#!/usr/bin/env bash
set -euo pipefail

PREFIX="${CMAKE_PREFIX_PATH:-/opt/shoots}"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${PREFIX}"
cmake --build build

./build/ShootsHost &
HOST_PID=$!
trap 'kill ${HOST_PID} >/dev/null 2>&1 || true' EXIT

sleep 1
curl -fsS http://127.0.0.1:8787/health | tee /tmp/shoots-host-health.json
kill ${HOST_PID} >/dev/null 2>&1 || true
wait ${HOST_PID} 2>/dev/null || true
