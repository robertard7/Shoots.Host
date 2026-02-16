#!/usr/bin/env bash
set -euo pipefail

PREFIX="${CMAKE_PREFIX_PATH:-/opt/shoots}"
DEPS_DIR="${TMPDIR:-/tmp}/Shoots.Provider"
BUILD_DIR="${DEPS_DIR}/build"

if command -v apt-get >/dev/null 2>&1; then
  sudo apt-get update
  sudo apt-get install -y cmake ninja-build build-essential git
fi

if [ ! -d "${DEPS_DIR}" ]; then
  git clone --depth 1 https://github.com/robertard7/Shoots.Provider.git "${DEPS_DIR}"
fi

cmake -S "${DEPS_DIR}" -B "${BUILD_DIR}" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${PREFIX}"
cmake --build "${BUILD_DIR}"
cmake --install "${BUILD_DIR}"

CONFIG_PATH="$(find "${PREFIX}" -name ShootsProviderConfig.cmake | head -n 1)"
if [ -z "${CONFIG_PATH}" ]; then
  echo "ShootsProviderConfig.cmake not found under ${PREFIX}" >&2
  exit 1
fi

echo "Installed ShootsProviderConfig.cmake: ${CONFIG_PATH}"
