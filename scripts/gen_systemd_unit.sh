#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

PREFIX="${SHOOTS_PREFIX:-/opt/shoots}"
BIND="${SHOOTS_HOST_BIND:-127.0.0.1}"
PORT="${SHOOTS_HOST_PORT:-8787}"
LOG_LEVEL="${SHOOTS_HOST_LOG_LEVEL:-info}"
LOG_FORMAT="${SHOOTS_HOST_LOG_FORMAT:-text}"
MAX_BODY_BYTES="${SHOOTS_HOST_MAX_BODY_BYTES:-1048576}"
REQ_TIMEOUT_MS="${SHOOTS_HOST_REQ_TIMEOUT_MS:-1000}"
MAX_INFLIGHT="${SHOOTS_HOST_MAX_INFLIGHT:-64}"
SHUTDOWN_DRAIN_MS="${SHOOTS_HOST_SHUTDOWN_DRAIN_MS:-2000}"
METRICS_FORMAT="${SHOOTS_HOST_METRICS_FORMAT:-json}"
API_KEY="${SHOOTS_HOST_API_KEY:-}"
CORS_ORIGIN="${SHOOTS_HOST_CORS_ORIGIN:-}"

cat <<UNIT
[Unit]
Description=Shoots Host
After=network.target

[Service]
Type=simple
ExecStart=${PREFIX}/bin/shoots-host --bind ${BIND} --port ${PORT} --log-level ${LOG_LEVEL} --log-format ${LOG_FORMAT} --max-body-bytes ${MAX_BODY_BYTES} --req-timeout-ms ${REQ_TIMEOUT_MS} --max-inflight ${MAX_INFLIGHT} --shutdown-drain-ms ${SHUTDOWN_DRAIN_MS} --metrics-format ${METRICS_FORMAT}
Environment=SHOOTS_HOST_API_KEY=${API_KEY}
Environment=SHOOTS_HOST_CORS_ORIGIN=${CORS_ORIGIN}
Restart=on-failure
RestartSec=1s

[Install]
WantedBy=multi-user.target
UNIT
