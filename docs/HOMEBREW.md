# Homebrew Formula Guidance (doc-only)

Shoots.Host does not publish an official Homebrew tap in this repository, but downstream operators can package it with a custom formula.

## Recommended formula behavior
- Build from a tagged release tarball.
- Build with CMake + Ninja.
- Install the host binary and runtime assets into Homebrew prefix paths.
- Do **not** use git submodules.

## Suggested install layout mapping
- `bin/shoots-host`
- `share/shoots-host/THIRD_PARTY_NOTICES.txt`
- `share/shoots-host/run_host.sh`

## Build flags
Use deterministic release-style flags aligned with project docs:

```ruby
system "cmake", "-S", ".", "-B", "build",
               "-G", "Ninja",
               "-DCMAKE_BUILD_TYPE=Release",
               "-DCMAKE_PREFIX_PATH=#{prefix}"
system "cmake", "--build", "build", "--config", "Release"
system "cmake", "--install", "build", "--prefix", prefix
```

## Caveats for formula maintainers
- `Shoots.Provider` must be installed and discoverable via `CMAKE_PREFIX_PATH`.
- Endpoint auth/TLS behavior is unchanged: terminate TLS at a reverse proxy.
- Keep default bind on loopback unless explicitly overridden.

## Smoke check examples
```bash
shoots-host --version
shoots-host --print-config
```

