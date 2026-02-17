# Changelog

## v0.1.0

## Added
- `--print-config` and `--dry-run` runtime controls.
- `/livez`, `/metrics`, `/status` deterministic operational endpoints.
- `X-Request-Id` response header and deterministic request IDs.
- `scripts/package_host.sh` and `scripts/release_check.sh`.
- `docs/CONTRACT.md` and `docs/INSTALL.md`.

## Changed
- Default bind is loopback `127.0.0.1`.
- Install layout uses `shoots-host` binary and `share/shoots-host` docs/scripts.
- Golden snapshots expanded to cover readiness/status/auth/limit/config contracts.

## Fixed
- Legacy path guard added to prevent regressions.
- In-flight request limit returns deterministic `429` envelope.


## Commits
- 3b4cecd
- 8d203f5
- cecab0e
- 5d8c672
- 7b5e54f
