# Shoots.Host

Shoots.Host is a transport-neutral HTTP host for `Shoots.Provider`.

## Design constraints

- `Shoots.Provider` is the authority for model behavior, capabilities, templates, and limits.
- Provider progress is host-driven via explicit polling; provider remains deterministic and threadless.
- Host can use a blocking event loop only and keeps deterministic response ordering.
- Host response shape is deterministic through stable JSON key ordering and monotonic job IDs.

## API endpoints

- `GET /health`
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

## Build and run (Linux)

```bash
./scripts/setup_provider_prefix.sh
./scripts/build_host.sh
```

Manual configure/build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=/opt/shoots
cmake --build build
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

## Contract

See `CONTRACT.md` for deterministic guarantees, error envelope, and job lifecycle details.
