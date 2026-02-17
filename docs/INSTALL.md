# Install

## Build
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/shoots
cmake --build build --config Release
```

## Install
```bash
cmake --install build --prefix /opt/shoots
```

Layout:
- `/opt/shoots/bin/shoots-host`
- `/opt/shoots/share/shoots-host/THIRD_PARTY_NOTICES.txt`
- `/opt/shoots/share/shoots-host/run_host.sh`

## Run
```bash
/opt/shoots/bin/shoots-host --bind 127.0.0.1 --port 8787
```

## systemd snippet (optional)
```ini
[Service]
ExecStart=/opt/shoots/bin/shoots-host --bind 127.0.0.1 --port 8787
Restart=on-failure
```


## Reproducible package tarball
```bash
./scripts/package_host.sh
```

Notes:
- Tarball metadata is normalized (sorted paths, fixed owner/group, deterministic mtimes).
- Gzip output uses `-n` to strip timestamp/name from the gzip header.
- `SOURCE_DATE_EPOCH` can be set to override the default commit timestamp seed.


## Windows zip package
```powershell
./scripts/package_host.ps1
```

Expected artifact:
- `dist/shoots-host-win64-<version>.zip`

Run helper:
```powershell
./scripts/run_host.ps1
```
