#!/usr/bin/env bash
# =============================================================================
#  ARINC 615A Tool Suite - preflight check (Linux)
#
#  Checks every precondition in seconds and prints the exact fix, so a missing
#  prerequisite is never discovered part-way through a build.
#
#  Usage:  scripts/doctor.sh
#  Exit:   0 = ready to build, 1 = something needs attention
# =============================================================================
set -uo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLS="${REPO}/.toolchain"
PROBLEMS=0
WARNINGS=0

ok()   { printf '  \033[32m[ OK ]\033[0m   %s\n           %s\n' "$1" "$2"; }
bad()  { printf '  \033[31m[FAIL]\033[0m   %s\n           %s\n' "$1" "$2"; PROBLEMS=$((PROBLEMS+1)); }
warn() { printf '  \033[33m[WARN]\033[0m   %s\n           %s\n' "$1" "$2"; WARNINGS=$((WARNINGS+1)); }
info() { printf '  \033[36m[INFO]\033[0m   %s\n           %s\n' "$1" "$2"; }
fix()  { printf '           FIX: %s\n' "$1"; }

printf '\n==============================================================\n'
printf ' Preflight check\n'
printf '==============================================================\n\n'

# --- compiler ----------------------------------------------------------------
if command -v g++ >/dev/null 2>&1; then
  GV="$(g++ -dumpversion | cut -d. -f1)"
  if [ "${GV}" -ge 13 ]; then
    ok "C++ compiler" "g++ ${GV} ($(command -v g++)) - supports C++23"
  else
    warn "C++ compiler" "g++ ${GV} may not support C++23; 13 or newer recommended"
  fi
else
  bad "C++ compiler" "g++ not found"
  fix "scripts/install-deps.sh"
fi

# --- cmake -------------------------------------------------------------------
cmake_ver() { "$1" --version 2>/dev/null | head -1 | awk '{print $3}'; }
cmake_ok() {
  local v major minor
  v="$(cmake_ver "$1")" || return 1
  [ -n "${v}" ] || return 1
  major="${v%%.*}"; minor="${v#*.}"; minor="${minor%%.*}"
  [ "${major}" -gt 4 ] && return 0
  [ "${major}" -eq 4 ] && [ "${minor}" -ge 3 ]
}
CMAKE_FOUND=""
for c in "${TOOLS}/cmake/bin/cmake" cmake; do
  if command -v "${c}" >/dev/null 2>&1 || [ -x "${c}" ]; then
    if cmake_ok "${c}"; then CMAKE_FOUND="${c}"; break; fi
  fi
done
if [ -n "${CMAKE_FOUND}" ]; then
  ok "CMake >= 4.3" "${CMAKE_FOUND} ($(cmake_ver "${CMAKE_FOUND}"))"
elif command -v cmake >/dev/null 2>&1; then
  bad "CMake >= 4.3" "found $(cmake_ver cmake), which is too old"
  fix "scripts/install-deps.sh  (fetches the official binary into .toolchain/)"
else
  bad "CMake >= 4.3" "not found"
  fix "scripts/install-deps.sh"
fi

# --- ninja, git, pkg-config --------------------------------------------------
for tool in ninja git pkg-config; do
  if command -v "${tool}" >/dev/null 2>&1; then
    ok "${tool}" "$(command -v ${tool})"
  else
    bad "${tool}" "not found"
    fix "scripts/install-deps.sh"
  fi
done

# --- libraries ---------------------------------------------------------------
if command -v pkg-config >/dev/null 2>&1; then
  for pc in spdlog fmt; do
    if pkg-config --exists "${pc}" 2>/dev/null; then
      ok "${pc}" "$(pkg-config --modversion ${pc})"
    else
      warn "${pc}" "pkg-config cannot see it (may still be found by CMake)"
    fi
  done
  if pkg-config --exists libxml++-5.0 2>/dev/null; then
    ok "libxml++" "5.0 ($(pkg-config --modversion libxml++-5.0))"
  elif pkg-config --exists libxml++-2.6 2>/dev/null; then
    ok "libxml++" "2.6 ($(pkg-config --modversion libxml++-2.6))"
  else
    warn "libxml++" "not visible to pkg-config - required by the arinc_665 dependency"
    fix "scripts/install-deps.sh"
  fi
fi
if [ -d /usr/include/boost ] || ls /usr/include/*/boost/version.hpp >/dev/null 2>&1; then
  ok "Boost" "headers present"
else
  warn "Boost" "headers not found in the usual places"
  fix "scripts/install-deps.sh"
fi

# --- offline dependency sources ----------------------------------------------
VENDORED=0
for d in helper arinc-649 arinc_665 tftp commands; do
  [ -f "${REPO}/${d}/CMakeLists.txt" ] && VENDORED=$((VENDORED+1))
done
if [ "${VENDORED}" -eq 5 ]; then
  ok "Dependency sources" "all 5 vendored in-tree - configure works OFFLINE"
elif [ "${VENDORED}" -eq 0 ]; then
  info "Dependency sources" "not vendored - configure will clone from git.thomas-vogt.de"
  fix "scripts/fetch-deps.sh   (run once on a connected machine)"
else
  warn "Dependency sources" "${VENDORED} of 5 vendored - the rest will be cloned"
fi

# --- network -----------------------------------------------------------------
if [ "${VENDORED}" -ne 5 ]; then
  if getent hosts git.thomas-vogt.de >/dev/null 2>&1; then
    ok "git.thomas-vogt.de" "resolves"
  else
    warn "git.thomas-vogt.de" "does not resolve - configure will fail"
    fix "scripts/fetch-deps.sh on a connected machine, or check DNS"
  fi
fi

# --- disk --------------------------------------------------------------------
FREE="$(df -BG --output=avail "${REPO}" 2>/dev/null | tail -1 | tr -dc '0-9')"
if [ -n "${FREE}" ]; then
  if [ "${FREE}" -lt 3 ]; then
    warn "Disk space" "${FREE} GB free - at least 3 GB recommended"
  else
    ok "Disk space" "${FREE} GB free"
  fi
fi

printf '\n==============================================================\n'
if [ "${PROBLEMS}" -eq 0 ]; then
  if [ "${WARNINGS}" -eq 0 ]; then
    printf ' READY - run ./build.sh\n'
  else
    printf ' READY with %d warning(s) - run ./build.sh\n' "${WARNINGS}"
  fi
  printf '==============================================================\n'
  exit 0
fi
printf ' %d problem(s) must be fixed before building\n' "${PROBLEMS}"
printf '==============================================================\n'
exit 1
