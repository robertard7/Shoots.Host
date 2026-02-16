#!/usr/bin/env bash
set -euo pipefail

./build/ShootsHost &
pid=$!
trap 'kill ${pid} >/dev/null 2>&1 || true' EXIT

sleep 1
curl -fsS http://127.0.0.1:8787/health
curl -fsS http://127.0.0.1:8787/v1/models
