#!/usr/bin/env bash
# =============================================================================
#  ARINC 615A Tool Suite - one command: install dependencies, build, run.
#
#    ./build.sh                     install deps, build release, show --help
#    ./build.sh -c Find             ...then run with these arguments
#    ./build.sh debug -c Find       ...as a debug build
#    ./build.sh --no-run            install and build only
#
#  Linux counterpart of build.bat. Uses system libraries via the distro package
#  manager; no vcpkg, because the GCC presets carry no vcpkg toolchain file.
# =============================================================================
set -euo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CFG="release"
RUN=1

# first argument may select the configuration
case "${1:-}" in
  debug)    CFG="debug";   shift ;;
  release)  CFG="release"; shift ;;
  --no-run) RUN=0;         shift ;;
esac
[ "${1:-}" = "--no-run" ] && { RUN=0; shift; }

say() { printf '\n\033[1m%s\033[0m\n' "$*"; }

say "=============================================================="
say " ARINC 615A Tool Suite"
say " install dependencies  ->  build (${CFG})  ->  run"
say "=============================================================="

"${REPO}/scripts/install-deps.sh" || {
  printf '\nDependency installation failed. See docs/BUILD.md section 6.\n' >&2
  exit 1
}

"${REPO}/scripts/build.sh" "${CFG}" || {
  printf '\nBuild failed. See docs/BUILD.md section 6.\n' >&2
  exit 1
}

EXE="${REPO}/cmake-build-gcc-static-${CFG}/app/arinc_615a_operation/arinc_615a_operation"
if [ ! -x "${EXE}" ]; then
  printf '\nERROR: build reported success but %s is missing.\n' "${EXE}" >&2
  exit 1
fi

if [ "${RUN}" -eq 0 ]; then
  printf '\nBuilt: %s\n' "${EXE}"
  exit 0
fi

say "=============================================================="
say " Running"
say "=============================================================="
set +e
if [ "$#" -eq 0 ]; then
  "${EXE}" --help
else
  "${EXE}" "$@"
fi
RC=$?
set -e

printf '\n--------------------------------------------------------------\n'
printf ' Executable: %s\n' "${EXE}"
printf ' Run again directly:\n'
printf '   %s -c Find\n' "${EXE}"
printf -- '--------------------------------------------------------------\n'
exit "${RC}"
