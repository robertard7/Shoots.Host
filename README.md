# Shoots.Host

Shoots.Host is a transport-neutral HTTP host for `Shoots.Provider`.

## Design constraints

- `Shoots.Provider` is the authority for model behavior, capabilities, templates, and limits.
- Provider progress is host-driven via explicit polling; provider remains deterministic and threadless.
- Host can use OS threads if needed, but the default deployment target is single-process and single-server.
- Host response shape is deterministic through stable JSON key ordering and monotonic job IDs.

## API endpoints

- `GET /health`
- `GET /v1/models`
- `GET /v1/templates`
- `POST /v1/chat`
- `POST /v1/tool`
- `POST /v1/build`
- `GET /v1/jobs/{id}`
- `POST /v1/jobs/{id}/take`

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
./scripts/run_local.ps1
```

## Codex environment notes

Set these environment variables in Codex before running setup:

- `MISE_DISABLE=1`
- `MISE_ENV=disable`
- `MISE_LOG_LEVEL=error`
- `CMAKE_PREFIX_PATH=/opt/shoots`

Then run:

```bash
./codex/setup.sh
```

For maintenance rebuilds:

```bash
./codex/maintenance.sh
```

## Provider dependency

`Shoots.Provider` is discovered through CMake package config (`ShootsProviderConfig.cmake`).
If configure fails, install the provider and pass `-DCMAKE_PREFIX_PATH=<prefix>`.
