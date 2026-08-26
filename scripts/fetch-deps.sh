#!/usr/bin/env bash
# =============================================================================
#  Vendor the five FetchContent dependencies into the source tree.
#
#  Run this ONCE on a machine with network access. Afterwards CMake configure
#  never contacts git.thomas-vogt.de, because the top-level CMakeLists.txt
#  already prefers an in-tree checkout:
#
#      if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/helper )
#        set( FETCHCONTENT_SOURCE_DIR_HELPER ${CMAKE_SOURCE_DIR}/helper )
#      endif()
#
#  ...and likewise for arinc-649, arinc_665, tftp and commands. That hook is
#  built into the project; this script only populates it.
#
#  Usage:  scripts/fetch-deps.sh            clone or update all five
#          scripts/fetch-deps.sh --status   report what is vendored
# =============================================================================
set -uo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BASE="https://git.thomas-vogt.de/thomas-vogt"
DEPS="helper:helper.git arinc-649:arinc-649.git arinc_665:arinc_665.git tftp:tftp.git commands:commands.git"

if [ "${1:-}" = "--status" ]; then
  printf '\n Vendored dependency status\n --------------------------\n'
  n=0
  for pair in ${DEPS}; do
    d="${pair%%:*}"
    if [ -f "${REPO}/${d}/CMakeLists.txt" ]; then
      printf '  [ yes ] %s\n' "${d}"; n=$((n+1))
    else
      printf '  [  no ] %s\n' "${d}"
    fi
  done
  printf '\n %d of 5 vendored.  5 of 5 means configure runs fully offline.\n\n' "${n}"
  exit 0
fi

printf '\n==============================================================\n'
printf ' Vendoring dependencies for offline builds\n'
printf '==============================================================\n'
printf ' target: %s\n\n' "${REPO}"

rc=0
for pair in ${DEPS}; do
  d="${pair%%:*}"
  repo="${pair##*:}"
  if [ -d "${REPO}/${d}/.git" ]; then
    printf '  [update] %s\n' "${d}"
    git -C "${REPO}/${d}" pull --ff-only \
      || printf '           WARNING: update failed, keeping existing checkout\n'
  else
    printf '  [clone ] %s\n' "${d}"
    if ! git clone --depth 1 "${BASE}/${repo}" "${REPO}/${d}"; then
      printf '           ERROR: clone failed - %s/%s\n' "${BASE}" "${repo}"
      rc=1
    fi
  fi
done

printf '\n==============================================================\n'
if [ "${rc}" -eq 0 ]; then
  printf ' Done. Configure will now use the in-tree checkouts.\n'
  printf ' These directories are git-ignored: a local cache, not part\n'
  printf ' of this repository.\n'
else
  printf ' Finished with errors - see above.\n'
fi
printf '==============================================================\n'
exit "${rc}"
