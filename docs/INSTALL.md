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
- `/opt/shoots/share/shoots-host/THIRD_PARTY_NOTICES.md`
- `/opt/shoots/share/shoots-host/run_host.sh`

## Run
```bash
/opt/shoots/bin/shoots-host --bind 127.0.0.1 --port 8787
```
