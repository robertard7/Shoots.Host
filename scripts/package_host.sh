#!/usr/bin/env bash
set -euo pipefail

export GIT_TERMINAL_PROMPT=0
export GIT_ASKPASS=/bin/true

PREFIX="${SHOOTS_PREFIX:-/opt/shoots}"
VERSION="${SHOOTS_HOST_VERSION:-0.1.0}"
STAGE="build/package-stage"
DIST_DIR="dist"

if [ -n "${SOURCE_DATE_EPOCH:-}" ]; then
  SOURCE_EPOCH="$SOURCE_DATE_EPOCH"
else
  SOURCE_EPOCH="$(git log -1 --pretty=%ct)"
fi

SOURCE_MTIME="$(date -u -d "@${SOURCE_EPOCH}" '+%Y-%m-%d %H:%M:%S')"
SOURCE_TOUCH="$(date -u -d "@${SOURCE_EPOCH}" '+%Y%m%d%H%M.%S')"

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$PREFIX"
cmake --build build --config Release

rm -rf "$STAGE"
cmake --install build --prefix "$STAGE"

LC_ALL=C find "$STAGE" -print0 | xargs -0 touch -h -t "$SOURCE_TOUCH"

mkdir -p "$DIST_DIR"
TARBALL="$DIST_DIR/shoots-host-$VERSION.tar.gz"

LC_ALL=C tar --sort=name \
  --mtime="UTC ${SOURCE_MTIME}" \
  --owner=0 \
  --group=0 \
  --numeric-owner \
  -cf - \
  -C "$STAGE" . \
  | gzip -n > "$TARBALL"

echo "$TARBALL"
