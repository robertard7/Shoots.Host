#!/usr/bin/env bash
set -euo pipefail

./build/ShootsHost &
pid=$!
trap 'kill ${pid} >/dev/null 2>&1 || true' EXIT

sleep 1

health_json="$(curl -fsS http://127.0.0.1:8787/health)"
echo "${health_json}" | grep -F '"ok":true'

caps_json="$(curl -fsS http://127.0.0.1:8787/v1/caps)"
echo "${caps_json}" | grep -F '"maxPayloadBytes"'

submit_json="$(curl -fsS -X POST http://127.0.0.1:8787/v1/chat -H 'content-type: application/json' -d '{"payload":"hello"}')"
echo "${submit_json}" | grep -F '"jobId"'
job_id="$(echo "${submit_json}" | sed -n 's/.*"jobId":"\([0-9][0-9]*\)".*/\1/p')"
if [ -z "${job_id}" ]; then
  echo "missing job id" >&2
  exit 1
fi

job_json="$(curl -fsS "http://127.0.0.1:8787/v1/jobs/${job_id}")"
echo "${job_json}" | grep -F '"status"'

take_json="$(curl -fsS -X POST "http://127.0.0.1:8787/v1/jobs/${job_id}/take")"
echo "${take_json}" | grep -F '"result"'
