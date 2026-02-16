# Shoots.Host

Shoots.Host is a transport-neutral HTTP host for `Shoots.Provider`.

## Design constraints

- `Shoots.Provider` is the authority for model behavior, capabilities, templates, and limits.
- Provider progress is host-driven via explicit polling; provider remains deterministic and threadless.
- Host can use OS threads if needed, but the default deployment target is single-process and single-server.
- Host response shape should remain deterministic (stable key ordering and stable request→job mapping as features are added).

## Endpoints

Current endpoints:

- `GET /health`
- `GET /v1/models` (planned)
- `GET /v1/capabilities` (planned)
- `GET /v1/templates` (planned)
- `POST /v1/jobs/*` (planned)
- `GET /v1/jobs/*` (planned)

## Build

### Linux (Ninja)

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Windows (MSVC)

```powershell
cmake -S . -B build
cmake --build build --config Release
```

## Provider dependency

`Shoots.Provider` is discovered using:

```cmake
find_package(ShootsProvider CONFIG REQUIRED)
```

Set `CMAKE_PREFIX_PATH` (or equivalent environment/toolchain configuration) so CMake can locate the provider package.

## Codex environment notes

- This repository is intended to build in CI (Ubuntu + Windows) and in local developer environments.
- `cpp-httplib` is vendored under `external/cpp-httplib`.
- The host currently exposes `SHOOTS_HOST_PORT` for basic port selection.
