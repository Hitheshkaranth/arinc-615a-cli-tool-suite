#!/usr/bin/env bash
# =============================================================================
#  ARINC 615A Tool Suite - dependency installer (Linux)
#
#    1. installs the toolchain and system libraries with the distro's package
#       manager
#    2. ensures CMake >= 4.3, fetching the official binary if the distro ships
#       something older (most do)
#
#  Usage:  scripts/install-deps.sh
#
#  Unlike the Windows path this uses system libraries, not vcpkg: the GCC and
#  Clang presets carry no vcpkg toolchain file.
# =============================================================================
set -euo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLS="${REPO}/.toolchain"
CMAKE_MIN_MAJOR=4
CMAKE_MIN_MINOR=3
CMAKE_VERSION="${CMAKE_VERSION:-4.4.2}"

say()  { printf '\n\033[1m%s\033[0m\n' "$*"; }
info() { printf '  %s\n' "$*"; }
die()  { printf '\n\033[31mERROR:\033[0m %s\n' "$*" >&2; exit 1; }

SUDO=""
if [ "$(id -u)" -ne 0 ]; then
  command -v sudo >/dev/null 2>&1 && SUDO="sudo" || die "not root and sudo not found"
fi

say "ARINC 615A Tool Suite - dependency installer (Linux)"
info "repo: ${REPO}"

# ---------------------------------------------------------------- packages --
say "[1/2] Installing toolchain and libraries"

if   command -v apt-get >/dev/null 2>&1; then PM=apt
elif command -v dnf     >/dev/null 2>&1; then PM=dnf
elif command -v pacman  >/dev/null 2>&1; then PM=pacman
elif command -v zypper  >/dev/null 2>&1; then PM=zypper
else die "no supported package manager found (apt/dnf/pacman/zypper)"; fi
info "package manager: ${PM}"

case "${PM}" in
  apt)
    $SUDO apt-get update
    # libxml++ is needed by the arinc_665 dependency, which does
    #   pkg_search_module( ... libxml++-5.0 )
    # so it must be the 5.0 series specifically - 2.6 does not provide that
    # .pc file and configure fails with "None of the required 'libxml++-5.0'
    # found". The package is spelled with a hyphen on Debian/Ubuntu
    # (libxml++-5.0-dev); the unhyphenated form is accepted as a fallback.
    XMLPP=""
    for cand in libxml++-5.0-dev libxml++5.0-dev; do
      if apt-cache show "${cand}" >/dev/null 2>&1; then XMLPP="${cand}"; break; fi
    done
    [ -n "${XMLPP}" ] || die "no libxml++ 5.0 development package available; arinc_665 requires libxml++-5.0"
    info "libxml++ package: ${XMLPP}"
    # Only the Boost components this project actually uses. libboost-all-dev
    # would pull in 200+ packages including MPI, NumPy and ROCm/HIP.
    $SUDO apt-get install -y --no-install-recommends \
      build-essential g++ ninja-build git pkg-config ca-certificates curl tar \
      libboost-dev \
      libboost-program-options-dev libboost-serialization-dev \
      libboost-test-dev libboost-system-dev libboost-thread-dev \
      libboost-filesystem-dev libboost-iostreams-dev \
      libspdlog-dev libfmt-dev "${XMLPP}"
    ;;
  dnf)
    $SUDO dnf install -y \
      gcc-c++ ninja-build git pkgconf-pkg-config ca-certificates curl tar \
      boost-devel spdlog-devel fmt-devel libxml++-devel
    ;;
  pacman)
    $SUDO pacman -Sy --needed --noconfirm \
      base-devel ninja git pkgconf curl tar boost spdlog fmt libxml++
    ;;
  zypper)
    $SUDO zypper --non-interactive install -y \
      gcc-c++ ninja git pkg-config curl tar \
      boost-devel spdlog-devel fmt-devel libxml++-devel
    ;;
esac

# GCC must understand C++23.
if command -v g++ >/dev/null 2>&1; then
  GCC_MAJOR="$(g++ -dumpversion | cut -d. -f1)"
  info "g++ major version: ${GCC_MAJOR}"
  [ "${GCC_MAJOR}" -ge 13 ] || \
    printf '  \033[33mWARNING:\033[0m g++ %s may not support C++23; 13+ recommended.\n' "${GCC_MAJOR}"
fi

# ------------------------------------------------------------------- cmake --
say "[2/2] Checking CMake >= ${CMAKE_MIN_MAJOR}.${CMAKE_MIN_MINOR}"

cmake_ok() {
  local exe="$1" v major minor
  command -v "${exe}" >/dev/null 2>&1 || return 1
  v="$("${exe}" --version 2>/dev/null | head -1 | awk '{print $3}')" || return 1
  major="${v%%.*}"; minor="${v#*.}"; minor="${minor%%.*}"
  [ -n "${major}" ] && [ -n "${minor}" ] || return 1
  [ "${major}" -gt "${CMAKE_MIN_MAJOR}" ] && return 0
  [ "${major}" -eq "${CMAKE_MIN_MAJOR}" ] && [ "${minor}" -ge "${CMAKE_MIN_MINOR}" ]
}

CMAKE_EXE=""
if cmake_ok cmake; then
  CMAKE_EXE="$(command -v cmake)"
elif cmake_ok "${TOOLS}/cmake/bin/cmake"; then
  CMAKE_EXE="${TOOLS}/cmake/bin/cmake"
else
  info "distro CMake is too old or absent; fetching ${CMAKE_VERSION} into .toolchain/"
  ARCH="$(uname -m)"
  case "${ARCH}" in
    x86_64|amd64)  CMARCH="linux-x86_64" ;;
    aarch64|arm64) CMARCH="linux-aarch64" ;;
    *) die "no official CMake binary for ${ARCH}; install CMake >= ${CMAKE_MIN_MAJOR}.${CMAKE_MIN_MINOR} manually" ;;
  esac
  TARBALL="cmake-${CMAKE_VERSION}-${CMARCH}.tar.gz"
  URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${TARBALL}"
  mkdir -p "${TOOLS}"
  curl -fsSL "${URL}" -o "${TOOLS}/${TARBALL}" \
    || die "download failed: ${URL}"
  rm -rf "${TOOLS}/cmake"
  mkdir -p "${TOOLS}/cmake"
  tar -xzf "${TOOLS}/${TARBALL}" -C "${TOOLS}/cmake" --strip-components=1
  rm -f "${TOOLS}/${TARBALL}"
  cmake_ok "${TOOLS}/cmake/bin/cmake" || die "fetched CMake still does not satisfy the minimum"
  CMAKE_EXE="${TOOLS}/cmake/bin/cmake"
fi
info "cmake: ${CMAKE_EXE} ($("${CMAKE_EXE}" --version | head -1 | awk '{print $3}'))"

say "Dependencies installed successfully."
info "Next step: scripts/build.sh [debug|release]"
