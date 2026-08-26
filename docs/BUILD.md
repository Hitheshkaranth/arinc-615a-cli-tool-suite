# Building the ARINC 615A Tool Suite

This document describes every dependency the project needs, how to install them,
what the build actually does at each stage, and the failure modes that are known
to bite on Windows.

If you just want the executable:

```bat
scripts\install-deps.bat
scripts\build.bat release
```

Everything below explains what those two commands do and why.

---

## 1. What gets built

| Target | Kind | Notes |
| --- | --- | --- |
| `arinc_615a` | library | Core protocol: files, FIND, host/target state machines, TFTP glue |
| `arinc_615a_commands` | static library | Command-registry wrappers for each operation |
| `arinc_615a_operation` | **executable** | The CLI. This is the deliverable. |
| `arinc_615a_test` | object library | `EXCLUDE_FROM_ALL`; only compiled if a test app is added |

`app/CMakeLists.txt` adds only `arinc_615a_operation`. The upstream project also
contains `arinc_615a_download_request_file`, `arinc_615a_test_tha` and
`arinc_615a_unit_test`; those sources are **not** part of this repository and are
not referenced by the build.

---

## 2. Dependencies

### 2.1 Toolchain (must be installed on the machine)

| Dependency | Minimum | Why | Install |
| --- | --- | --- | --- |
| Visual Studio C++ build tools | VS 2022 (MSVC 14.4x) | Compiler, linker, Windows SDK | `winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"` |
| CMake | **4.3** | `cmake_minimum_required( VERSION 4.3 )` in every `CMakeLists.txt` | `winget install --id Kitware.CMake -e` |
| Ninja | any recent | Generator used by all MSVC presets | Ships with the VS "C++ CMake tools for Windows" component |
| Git | any recent | vcpkg clone **and** `FetchContent` (see 2.3) | `winget install --id Git.Git -e` |

The compiler must support **C++23** — every target sets
`target_compile_features( … cxx_std_23 )`.

### 2.2 Library dependencies (installed by vcpkg from `vcpkg.json`)

```
boost-asio        boost-crc        boost-endian      boost-exception
boost-hash2       boost-multi-index boost-program-options
boost-property-tree boost-serialization boost-signals2 boost-test
libxmlpp          pkgconf          spdlog
```

Resolved transitively to ~84 packages, including `libxml2`, `libiconv`, `zlib`,
and `fmt`. You do not install these by hand — `scripts\install-deps.bat` does it.

### 2.3 Sibling projects (fetched automatically during configure)

The top-level `CMakeLists.txt` pulls five upstream repositories with
`FetchContent`, at tag `main`:

| Project | Repository |
| --- | --- |
| `helper` | `https://git.thomas-vogt.de/thomas-vogt/helper.git` |
| `arinc_649` | `https://git.thomas-vogt.de/thomas-vogt/arinc-649.git` |
| `arinc_665` | `https://git.thomas-vogt.de/thomas-vogt/arinc_665.git` |
| `tftp` | `https://git.thomas-vogt.de/thomas-vogt/tftp.git` |
| `commands` | `https://git.thomas-vogt.de/thomas-vogt/commands.git` |

**The configure step needs network access to `git.thomas-vogt.de`.** There is no
vendored copy and no offline fallback. If you need one, place a checkout beside
the source tree — CMake honours a sibling directory of the matching name and
sets `FETCHCONTENT_SOURCE_DIR_<NAME>` for you.

---

## 3. What `scripts\install-deps.bat` does

1. **Locates the C++ toolset** via `vswhere.exe`, requiring the
   `VC.Tools.x86.x64` component. Prints the exact `winget` line and stops if absent.
2. **Verifies CMake and Ninja.** Installs CMake through winget if missing; falls
   back to the Ninja that ships inside the VS installation.
3. **Clones and bootstraps vcpkg** into `C:\vcpkg` (shallow clone).
4. **Installs the manifest dependencies** for triplet `x64-windows`, with all
   working roots redirected to short paths.

### Why the short paths are not optional

vcpkg builds `libiconv` with autotools. During `make install` it forms a path by
concatenating the package directory **with the full install prefix**:

```
<packages>/libiconv_x64-windows/<entire install prefix>/debug/lib/iconv.lib
```

Under a normal project location that comfortably exceeds the 260-character
limit. libtool then falls back to the `\\?\` long-path form, which `cl.exe` does
not accept in that argument position:

```
cl : Command line warning D9002 : ignoring unknown option '//?/C:/…/iconv.lib'
iconv.obj : error LNK2019: unresolved external symbol libiconv_open
.libs\iconv.exe : fatal error LNK1120: 7 unresolved externals
```

The import library is silently dropped and the port fails — after roughly 40
minutes of work. The installer therefore pins:

| Root | Path |
| --- | --- |
| vcpkg root | `C:\vcpkg` |
| buildtrees | `C:\vb` |
| packages | `C:\vp` |
| installed | `C:\vi` |

Change these only if the replacements are equally short.

---

## 4. What `scripts\build.bat` does

```bat
scripts\build.bat [debug|release]      REM default: release
```

1. **Enters the MSVC environment** (`vcvars64.bat`) and puts VS's Ninja on `PATH`.
2. **Configures** with preset `msvc-static-debug` / `msvc-static-release`, plus
   three overrides:
   - `VCPKG_INSTALLED_DIR=C:/vi`
   - `VCPKG_INSTALL_OPTIONS=--x-buildtrees-root=C:/vb;--x-packages-root=C:/vp`
   - `CMAKE_COMPILE_WARNING_AS_ERROR=OFF` — see 6.2
3. **Builds every target** into `cmake-build-msvc-static-<config>\`.
4. **Prints the executable path** and the `PATH` line needed to run it.

### Configure stage, in order

1. vcpkg manifest install (no-op once `C:\vi` is populated)
2. MSVC compiler detection
3. `FetchContent` clone of the five sibling repos into `_deps/`
4. `find_package( Boost )`, `find_package( spdlog )`, libxml++ via pkgconf
5. `configure_file( Version.hpp.in Version.hpp )`
6. Generate `build.ninja`

On a cold machine the vcpkg step dominates — expect well over an hour, most of it
`libiconv`. Once the binary cache at `%LOCALAPPDATA%\vcpkg\archives` is warm,
configure drops to a couple of minutes.

### Build stage

Ninja compiles ~214 objects: the five fetched dependencies first, then
`arinc_615a`, then `arinc_615a_commands`, then links
`app\arinc_615a_operation\arinc_615a_operation.exe`.

---

## 5. Running the result

The build links against the **dynamic** `x64-windows` triplet, and the presets set
`VCPKG_APPLOCAL_DEPS=OFF`, so dependency DLLs are **not** copied next to the
executable. Launching it from Explorer will fail with a missing-DLL error.

```bat
REM release
set "PATH=C:\vi\x64-windows\bin;%PATH%"
REM debug  (must match the build configuration - do not mix)
set "PATH=C:\vi\x64-windows\debug\bin;%PATH%"

cmake-build-msvc-static-release\app\arinc_615a_operation\arinc_615a_operation.exe --help
```

Available commands:

```
AdhocUpload   BatchUpload   Create          Find
ImportMediaSet ImportMediaSetXml Information ListBatches
ListLoads     ListMediaSets MedDownload     OpDownload
RemoveMediaSet Targets      Upload          UploadLoads
```

A self-contained smoke test that needs no target hardware:

```bat
arinc_615a_operation.exe -c Find
```

It broadcasts a FIND request to `255.255.255.255`, waits out its 3-second
timeout, reports no targets, and exits 0.

### Two CLI option-parsing quirks

Both are in how the command registry declares options, not in the build:

- **Use `--option=value`, not `--option value`** for subcommand options.
  `-t 5` fails with *"the required argument for option '--timeout' is missing"*;
  `--timeout=5` works.
- **`-l` is bound twice inside `Find`** — to both `--log-level` and
  `--targets-list`. Passing `--log-level info` makes the parser consume the *next
  flag* as its value. Avoid `-l` on that subcommand.

---

## 6. Known failure modes

### 6.1 `libiconv` fails with `LNK2019` / `LNK1120`

Path length. See section 3. Verify vcpkg's roots are the short ones; a stray
`VCPKG_ROOT` pointing at a deep directory reintroduces it.

`libiconv` can legitimately take **hours** on its two autotools configure passes,
with the top-level log silent the whole time — vcpkg buffers a port's output
until the port finishes. To check real progress, watch file timestamps under
`C:\vb\libiconv\`, not the console.

### 6.2 `error C2220` from inside a Boost header

```
boost/exception/detail/type_info.hpp(240): error C2220: the following warning is treated as an error
boost/exception/detail/type_info.hpp(240): warning C4127: conditional expression is constant
```

The presets set `CMAKE_COMPILE_WARNING_AS_ERROR=True` **and**
`/external:templates-`. The latter re-enables warnings for templates instantiated
from project code, so a warning inside Boost's own headers becomes a hard error
while compiling the `tftp` dependency. `scripts\build.bat` passes
`-DCMAKE_COMPILE_WARNING_AS_ERROR=OFF`; the preset files are left untouched.

### 6.3 `No target "arinc_615a_download_request_file"`

`InstallPackage.cmake` referenced `$<TARGET_FILE_DIR:…>` for an application that
`app/CMakeLists.txt` no longer builds, which fails at *generate* time — after a
successful configure — once per entry in the runtime-dependency-set loop. Fixed
here by guarding it with `if( TARGET … )`.

### 6.4 Configure cannot reach `git.thomas-vogt.de`

The five `FetchContent` dependencies have no vendored copy. Without network
access to that host, configure cannot complete. See 2.3.

---

## 7. Linux

The Linux path differs from Windows in one important way: **it uses system
libraries, not vcpkg.** The GCC and Clang presets carry no vcpkg toolchain file,
so dependencies come from the distro package manager. That makes it far quicker
than the Windows first run — minutes, not hours — and `libiconv` never gets
built from source, so the long-path failure in section 6.1 cannot occur.

### One command

```bash
./build.sh                  # install deps, build release, show --help
./build.sh -c Find          # ...then run with these arguments
./build.sh debug -c Find    # ...as a debug build
./build.sh --no-run         # install and build only
```

Or the steps separately:

```bash
scripts/install-deps.sh
scripts/build.sh release
```

### What `scripts/install-deps.sh` installs

Supported package managers: **apt**, **dnf**, **pacman**, **zypper**.

| Need | apt | dnf | pacman | zypper |
| --- | --- | --- | --- | --- |
| Compiler | `build-essential g++` | `gcc-c++` | `base-devel` | `gcc-c++` |
| Generator | `ninja-build` | `ninja-build` | `ninja` | `ninja` |
| Boost | `libboost-all-dev` | `boost-devel` | `boost` | `boost-devel` |
| spdlog | `libspdlog-dev` | `spdlog-devel` | `spdlog` | `spdlog-devel` |
| fmt | `libfmt-dev` | `fmt-devel` | `fmt` | `fmt-devel` |
| libxml++ | `libxml++5.0-dev`, falling back to `libxml++2.6-dev` | `libxml++-devel` | `libxml++` | `libxml++-devel` |
| Misc | `git pkg-config curl tar ca-certificates` | same | same | same |

`libxml++` is required by the `arinc_665` dependency, not by this repository's
own libraries — `lib/arinc_615a` only calls `find_package( Boost )` and
`find_package( spdlog )`.

### CMake on Linux

Every `CMakeLists.txt` here declares `cmake_minimum_required( VERSION 4.3 )`, and
**most distributions ship something older**. The installer therefore:

1. uses the system `cmake` if it already satisfies `>= 4.3`; otherwise
2. downloads the official Kitware binary for `x86_64` or `aarch64` into
   `.toolchain/cmake/` inside the repository, and uses that.

`.toolchain/` is git-ignored. Override the version with
`CMAKE_VERSION=4.4.2 scripts/install-deps.sh`. On any other architecture the
script stops and asks you to install CMake >= 4.3 yourself.

### Compiler requirement

**GCC 13 or newer**, because every target sets `cxx_std_23`. The installer prints
a warning if `g++ -dumpversion` reports less than 13 rather than failing, since
some distributions backport enough of C++23 to work.

### Running the result

No `PATH` juggling is needed — the executable links against system libraries in
the normal loader search path:

```bash
cmake-build-gcc-static-release/app/arinc_615a_operation/arinc_615a_operation --help
cmake-build-gcc-static-release/app/arinc_615a_operation/arinc_615a_operation -c Find
```

### Warnings as errors

`scripts/build.sh` passes `-DCMAKE_COMPILE_WARNING_AS_ERROR=OFF` for the same
reason the Windows script does: the presets combine `-Werror` with `-Wall
-Wextra -Wpedantic`, and a warning raised inside a dependency's headers would
otherwise fail the build.

### Not verified

The Linux scripts were written against the presets and the distro package names
but **were not executed** — this work was done on a Windows machine with no
Linux environment available. They are syntax-checked (`bash -n`) and their
argument handling was exercised, but the package installs, the CMake download
and the compile itself have not been run. Treat the first Linux run as
unverified, and expect package-name drift across distribution releases to be the
most likely thing to need adjusting.

### Cross-compiling for Windows from Linux

`cmake/presets/CMakePresetsMinGwCross.json` provides MinGW cross presets using
`cmake/mingw64-cross-toolchain.cmake`. They come from upstream and are not
exercised by these scripts.

---

## 8. Other toolchains

Presets also exist for GCC, Clang and a MinGW cross-build
(`CMakePresetsGcc.json`, `CMakePresetsClang.json`, `CMakePresetsMinGwCross.json`),
each in static/shared × debug/release. They are carried over from upstream and
are **not** exercised by `scripts\build.bat`, which is Windows/MSVC only.

```bash
cmake --preset gcc-static-release
cmake --build cmake-build-gcc-static-release
```

---

## 9. Linked resources

Everything referenced by this build, in one place.

### Toolchain downloads

| Tool | Link | Install |
| --- | --- | --- |
| Visual Studio 2022 Build Tools | <https://visualstudio.microsoft.com/downloads/> | `winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"` |
| CMake (>= 4.3 required) | <https://cmake.org/download/> | `winget install --id Kitware.CMake -e` |
| Ninja | <https://ninja-build.org/> | Ships with the VS "C++ CMake tools for Windows" component |
| Git | <https://git-scm.com/downloads> | `winget install --id Git.Git -e` |
| vcpkg | <https://github.com/microsoft/vcpkg> | Cloned and bootstrapped by `scripts\install-deps.bat` |

### vcpkg reference

- Manifest mode (`vcpkg.json`) — <https://learn.microsoft.com/vcpkg/consume/manifest-mode>
- Binary caching (`%LOCALAPPDATA%
cpkgrchives`) — <https://learn.microsoft.com/vcpkg/consume/binary-caching-overview>
- Triplets (`x64-windows` is dynamic) — <https://learn.microsoft.com/vcpkg/users/triplets>
- Build failure troubleshooting — <https://learn.microsoft.com/vcpkg/troubleshoot/build-failures>

### Sibling projects (fetched by `FetchContent`)

| Project | Repository | Role in this build |
| --- | --- | --- |
| `helper` | <https://git.thomas-vogt.de/thomas-vogt/helper> | Shared utilities, logging setup, exception tags |
| `arinc_649` | <https://git.thomas-vogt.de/thomas-vogt/arinc-649> | `CheckValue` and check-value generators |
| `arinc_665` | <https://git.thomas-vogt.de/thomas-vogt/arinc_665> | Media sets, load headers — the `Create`/`Import`/`List*` commands |
| `tftp` | <https://git.thomas-vogt.de/thomas-vogt/tftp> | The TFTP client/server the protocol rides on |
| `commands` | <https://git.thomas-vogt.de/thomas-vogt/commands> | `CommandRegistry` and command-line dispatch |
| upstream | <https://git.thomas-vogt.de/thomas-vogt/arinc_615a> | The project this repository is trimmed from |

### Library dependencies

| Library | Home |
| --- | --- |
| Boost | <https://www.boost.org/> |
| Boost.Asio | <https://www.boost.org/doc/libs/release/doc/html/boost_asio.html> |
| Boost.Program_options | <https://www.boost.org/doc/libs/release/doc/html/program_options.html> |
| libxml++ | <https://libxmlplusplus.github.io/libxmlplusplus/> |
| spdlog | <https://github.com/gabime/spdlog> |
| fmt | <https://fmt.dev/> |
| libxml2 | <https://gitlab.gnome.org/GNOME/libxml2> |
| libiconv | <https://www.gnu.org/software/libiconv/> |
| zlib | <https://zlib.net/> |

### CMake reference

- `cmake-presets(7)` — <https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html>
- `FetchContent` — <https://cmake.org/cmake/help/latest/module/FetchContent.html>
- `generate_export_header` — <https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html>
- `install(RUNTIME_DEPENDENCY_SET)` — <https://cmake.org/cmake/help/latest/command/install.html#runtime-dependency-set>

### MSVC reference — for the failure modes in section 6

- `/external:` warning control — <https://learn.microsoft.com/cpp/build/reference/external-external-headers-diagnostics>
- `/WX` warnings as errors — <https://learn.microsoft.com/cpp/build/reference/compiler-option-warning-level>
- Warning C4127 — <https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4127>
- Error LNK2019 — <https://learn.microsoft.com/cpp/error-messages/tool-errors/linker-tools-error-lnk2019>
- Maximum path length and the `\\?\` long-path prefix — <https://learn.microsoft.com/windows/win32/fileio/maximum-file-path-limitation>

### Standards

| Standard | Title |
| --- | --- |
| ARINC 615A-4 | Software Data Loader Using Ethernet Interface |
| ARINC 665-5 | Loadable Software Standards |
| ARINC 649 | Common Terminology and Functions for Software Distribution and Loading |
| RFC 1350 | The TFTP Protocol (Revision 2) — <https://www.rfc-editor.org/rfc/rfc1350> |
| RFC 2347 | TFTP Option Extension — <https://www.rfc-editor.org/rfc/rfc2347> |
| RFC 2348 | TFTP Blocksize Option — <https://www.rfc-editor.org/rfc/rfc2348> |
| RFC 2349 | TFTP Timeout Interval and Transfer Size Options — <https://www.rfc-editor.org/rfc/rfc2349> |

ARINC standards are published by SAE ITC and are not freely redistributable —
<https://www.aviation-ia.com/product-categories/arinc-standards>.

### Documents in this repository

| Document | Contents |
| --- | --- |
| [../README.md](../README.md) | Overview, diagrams, command list, quick start |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Layer map, directory-by-directory walkthrough, design conventions |
| [CODE-TRACE.md](CODE-TRACE.md) | Function-by-function trace from `main()` to the wire, with `file:line` |
| [BUILD.md](BUILD.md) | This document |

---

## 10. Licence

Mozilla Public License 2.0 — see [LICENSE](LICENSE). Upstream project by
Thomas Vogt, <https://git.thomas-vogt.de/thomas-vogt/arinc_615a>.
