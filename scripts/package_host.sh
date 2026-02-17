#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

PREFIX="${SHOOTS_PREFIX:-/opt/shoots}"
VERSION="${SHOOTS_HOST_VERSION:-0.1.0}"
STAGE="build/package-stage"
DIST_DIR="dist"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$PREFIX"
cmake --build build --config Release

rm -rf "$STAGE"
cmake --install build --prefix "$STAGE"

find "$STAGE" -type f -print0 | sort -z | xargs -0 touch -t 197001010000.00

mkdir -p "$DIST_DIR"
TARBALL="$DIST_DIR/shoots-host-$VERSION.tar.gz"

tar --sort=name --mtime='UTC 1970-01-01' --owner=0 --group=0 --numeric-owner -czf "$TARBALL" -C "$STAGE" .
echo "$TARBALL"
