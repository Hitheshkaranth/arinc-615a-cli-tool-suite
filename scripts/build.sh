#!/usr/bin/env bash
# =============================================================================
#  ARINC 615A Tool Suite - build script (Linux, GCC + Ninja)
#
#  Usage:  scripts/build.sh [debug|release]      (default: release)
#
#  Run scripts/install-deps.sh first.
# =============================================================================
set -euo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLS="${REPO}/.toolchain"
CFG="${1:-release}"

case "${CFG}" in
  debug|release) ;;
  *) printf 'ERROR: unknown configuration "%s". Use "debug" or "release".\n' "${CFG}" >&2; exit 1 ;;
esac

PRESET="gcc-static-${CFG}"
BUILDDIR="${REPO}/cmake-build-${PRESET}"

say()  { printf '\n\033[1m%s\033[0m\n' "$*"; }
info() { printf '  %s\n' "$*"; }

# Prefer a CMake that satisfies cmake_minimum_required( VERSION 4.3 ).
cmake_ok() {
  local exe="$1" v major minor
  command -v "${exe}" >/dev/null 2>&1 || return 1
  v="$("${exe}" --version 2>/dev/null | head -1 | awk '{print $3}')" || return 1
  major="${v%%.*}"; minor="${v#*.}"; minor="${minor%%.*}"
  [ -n "${major}" ] && [ -n "${minor}" ] || return 1
  [ "${major}" -gt 4 ] && return 0
  [ "${major}" -eq 4 ] && [ "${minor}" -ge 3 ]
}

if cmake_ok "${TOOLS}/cmake/bin/cmake"; then
  CMAKE_EXE="${TOOLS}/cmake/bin/cmake"
elif cmake_ok cmake; then
  CMAKE_EXE="$(command -v cmake)"
else
  printf 'ERROR: no CMake >= 4.3 found. Run scripts/install-deps.sh first.\n' >&2
  exit 1
fi

say "ARINC 615A Tool Suite - build"
info "configuration : ${CFG}"
info "preset        : ${PRESET}"
info "cmake         : ${CMAKE_EXE}"
info "build dir     : ${BUILDDIR}"

# CMAKE_COMPILE_WARNING_AS_ERROR is forced OFF: the presets combine -Werror
# with -Wall -Wextra -Wpedantic, and warnings raised inside dependency headers
# would otherwise fail the build. See docs/BUILD.md section 6.2.
say "[1/2] Configuring"
"${CMAKE_EXE}" --preset "${PRESET}" -DCMAKE_COMPILE_WARNING_AS_ERROR=OFF

say "[2/2] Building all targets"
"${CMAKE_EXE}" --build "${BUILDDIR}"

EXE="${BUILDDIR}/app/arinc_615a_operation/arinc_615a_operation"
say "BUILD OK"
if [ -x "${EXE}" ]; then
  info "Executable: ${EXE}"
  info "Run:        ${EXE} --help"
else
  printf '  WARNING: expected executable not found at %s\n' "${EXE}"
fi
