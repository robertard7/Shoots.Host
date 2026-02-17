# Shoots.Host

Shoots.Host is a transport-neutral HTTP host for `Shoots.Provider`.

## Design constraints

- `Shoots.Provider` is the authority for model behavior, capabilities, templates, and limits.
- Provider progress is host-driven via explicit polling; provider remains deterministic and threadless.
- Host can use a blocking event loop only and keeps deterministic response ordering.
- Host response shape is deterministic through stable JSON key ordering and monotonic request IDs.

## API endpoints

- `GET /`
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

## How to run

Build (Release):

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/shoots
cmake --build build --config Release
```

Run (env + flags):

```bash
export SHOOTS_HOST_PORT=8787
export SHOOTS_HOST_BIND=127.0.0.1
export SHOOTS_HOST_LOG_LEVEL=info
export SHOOTS_HOST_MAX_BODY_BYTES=1048576
export SHOOTS_HOST_REQ_TIMEOUT_MS=1000
export SHOOTS_HOST_API_KEY=
export SHOOTS_HOST_CORS_ORIGIN=
./build/shoots-host \
  --port "$SHOOTS_HOST_PORT" \
  --bind "$SHOOTS_HOST_BIND" \
  --log-level "$SHOOTS_HOST_LOG_LEVEL" \
  --max-body-bytes "$SHOOTS_HOST_MAX_BODY_BYTES" \
  --req-timeout-ms "$SHOOTS_HOST_REQ_TIMEOUT_MS" \
  --api-key "$SHOOTS_HOST_API_KEY" \
  --cors-origin "$SHOOTS_HOST_CORS_ORIGIN"
```

Print resolved config without starting:

```bash
./build/shoots-host --print-config
./build/shoots-host --dry-run
```

Example curl checks:

```bash
curl -s http://127.0.0.1:8787/healthz
curl -s http://127.0.0.1:8787/readyz
```

Troubleshooting:

- Missing provider prefix: set `CMAKE_PREFIX_PATH=/opt/shoots` and ensure `ShootsProviderConfig.cmake` exists under that prefix.
- Provider not ready: check startup logs for `provider.ready` and `provider.endpoint` values.
- API key enabled: send header `X-Api-Key: <key>` for non-health endpoints.

## Build and run helpers

```bash
./scripts/ci_local.sh
./scripts/run_host.sh
./scripts/install_host.sh
```

## Third-party dependency strategy

- `httplib` is vendored as `external/httplib/httplib.h`.

## Contract

See `CONTRACT.md` for deterministic guarantees, error envelope, and job lifecycle details.
