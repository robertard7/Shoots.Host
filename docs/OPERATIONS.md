# Operations

## Runtime environment
Recommended defaults:
- `SHOOTS_HOST_BIND=127.0.0.1`
- `SHOOTS_HOST_PORT=8787`
- `SHOOTS_HOST_LOG_LEVEL=info`
- `SHOOTS_HOST_LOG_FORMAT=text`
- `SHOOTS_HOST_MAX_INFLIGHT=64`
- `SHOOTS_HOST_METRICS_FORMAT=json`

## Start
```bash
./scripts/run_host.sh
```

## systemd example
Generate a unit matching current env/defaults:

```bash
./scripts/gen_systemd_unit.sh > shoots-host.service
```

Example output:
```ini
[Unit]
Description=Shoots Host
After=network.target

[Service]
ExecStart=/opt/shoots/bin/shoots-host --bind 127.0.0.1 --port 8787
Restart=on-failure
Environment=SHOOTS_HOST_LOG_LEVEL=info

[Install]
WantedBy=multi-user.target
```

## Upgrade steps
1. Build and package: `./scripts/package_host.sh`
2. Stop service
3. Install new build to prefix (`cmake --install ... --prefix /opt/shoots`)
4. Start service
5. Verify `/livez`, `/healthz`, `/readyz`


## TLS
Terminate TLS at a reverse proxy; keep Shoots.Host on loopback. See `docs/TLS.md`.

## Bounded load sanity check
Run a quick bounded overload check (expects some `429` responses):

```bash
./scripts/load_test_livez.sh
```

Tuning knobs:
- `SHOOTS_HOST_MAX_INFLIGHT` (default `1` for this harness)
- `SHOOTS_LOAD_CONCURRENCY` (default `64`)
- `SHOOTS_LOAD_ROUNDS` (default `20`)
