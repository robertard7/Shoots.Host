# Shoots.Host

Shoots.Host is a transport-neutral HTTP host for `Shoots.Provider`.

## Design constraints

- `Shoots.Provider` is the authority for model behavior, capabilities, templates, and limits.
- Provider progress is host-driven via explicit polling; provider remains deterministic and threadless.
- Host can use a blocking event loop only and keeps deterministic response ordering.
- Host response shape is deterministic through stable JSON key ordering and monotonic request IDs.

## API endpoints

- `GET /healthz`
- `GET /readyz`
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

## Deterministic request validation

Submit endpoints use strict object-shape validation.

- Required fields are enforced.
- Unknown top-level fields are rejected.
- `modelId` has a size cap.
- Over-size request body fails at host boundary.

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
./build/ShootsHost --port "$SHOOTS_HOST_PORT" --bind "$SHOOTS_HOST_BIND" --log-level "$SHOOTS_HOST_LOG_LEVEL"
```

Example curl checks:

```bash
curl -s http://127.0.0.1:8787/healthz
curl -s http://127.0.0.1:8787/readyz
```

Troubleshooting:

- Missing provider prefix: set `CMAKE_PREFIX_PATH=/opt/shoots` and ensure `ShootsProviderConfig.cmake` exists under that prefix.
- Provider not ready: check startup logs for `provider.ready` and `provider.endpoint` values.

## Build and run (Linux helper scripts)

```bash
export CMAKE_PREFIX_PATH=/opt/shoots
./scripts/setup_provider_prefix.sh
./scripts/build_host.sh
./scripts/run_host.sh
```

## Build and run (Windows)

```powershell
./scripts/win/build.ps1
./scripts/win/run.ps1
```

## Codex environment notes

Set:

- `CMAKE_PREFIX_PATH=/opt/shoots`

Then run:

```bash
./codex/setup.sh
```

For maintenance rebuilds:

```bash
./codex/maintenance.sh
```

Scripts are non-interactive (`GIT_TERMINAL_PROMPT=0`, `GIT_ASKPASS=/bin/true`).

## Third-party dependency strategy

- `cpp-httplib` is vendored as `external/httplib/httplib.h`.
- Codex setup does not require submodule checkout for HTTP hosting.

## Contract

See `CONTRACT.md` for deterministic guarantees, error envelope, and job lifecycle details.
