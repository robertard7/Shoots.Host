# Shoots.Host

Shoots.Host is a transport-neutral HTTP host for `Shoots.Provider`.

## API endpoints
- `GET /`
- `GET /livez`
- `GET /healthz`
- `GET /readyz`
- `GET /metrics`
- `GET /status`
- `GET /v1/caps`
- `GET /v1/models`
- `GET /v1/templates`
- `GET /v1/jobs?limit=N`
- `GET /v1/jobs/{id}`
- `GET /v1/jobs/{id}/wait?timeout_ms=...`
- `POST /v1/chat`
- `POST /v1/tool`
- `POST /v1/build`
- `POST /v1/jobs/{id}/take`

## Build
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/shoots
cmake --build build --config Release
```

## Run
```bash
export SHOOTS_HOST_PORT=8787
export SHOOTS_HOST_BIND=127.0.0.1
export SHOOTS_HOST_LOG_LEVEL=info
export SHOOTS_HOST_LOG_FORMAT=text
export SHOOTS_HOST_MAX_BODY_BYTES=1048576
export SHOOTS_HOST_REQ_TIMEOUT_MS=1000
export SHOOTS_HOST_API_KEY=
export SHOOTS_HOST_CORS_ORIGIN=
export SHOOTS_HOST_MAX_INFLIGHT=64
export SHOOTS_HOST_SHUTDOWN_DRAIN_MS=2000
export SHOOTS_HOST_METRICS_FORMAT=json
./build/shoots-host \
  --port "$SHOOTS_HOST_PORT" \
  --bind "$SHOOTS_HOST_BIND" \
  --log-level "$SHOOTS_HOST_LOG_LEVEL" \
  --log-format "$SHOOTS_HOST_LOG_FORMAT" \
  --max-body-bytes "$SHOOTS_HOST_MAX_BODY_BYTES" \
  --req-timeout-ms "$SHOOTS_HOST_REQ_TIMEOUT_MS" \
  --api-key "$SHOOTS_HOST_API_KEY" \
  --cors-origin "$SHOOTS_HOST_CORS_ORIGIN" \
  --max-inflight "$SHOOTS_HOST_MAX_INFLIGHT" \
  --shutdown-drain-ms "$SHOOTS_HOST_SHUTDOWN_DRAIN_MS" \
  --metrics-format "$SHOOTS_HOST_METRICS_FORMAT"
```

## Config validation
```bash
./build/shoots-host --print-config
./build/shoots-host --dry-run
```

## Quick checks
```bash
curl -s http://127.0.0.1:8787/livez
curl -s http://127.0.0.1:8787/healthz
curl -s http://127.0.0.1:8787/readyz
curl -s http://127.0.0.1:8787/metrics
```

## Helper scripts
```bash
./scripts/ci_local.sh
./scripts/run_host.sh
./scripts/install_host.sh
./scripts/package_host.sh
./scripts/release_check.sh
```

## Docs
- `docs/CONTRACT.md`
- `docs/INSTALL.md`
- `CHANGELOG.md`
- `SECURITY.md`
- `docs/OPERATIONS.md`
- `docs/TLS.md`
