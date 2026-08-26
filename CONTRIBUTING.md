# Contributing

## Before anything else

```bat
scripts\doctor.bat          REM Windows - checks every precondition in seconds
```
```bash
scripts/doctor.sh           # Linux
```

`doctor` tells you exactly what is missing and the command that fixes it. Run it
before opening an issue about a failing build.

---

## Local changes that must survive an upstream merge

This repository is a trimmed distribution of the
[upstream ARINC 615A Tool Suite](https://git.thomas-vogt.de/thomas-vogt/arinc_615a).
Three changes were needed to make it build, and each will be silently undone if
upstream files are copied over wholesale. **Re-apply them after any merge.**

| # | File | Change | Why |
| --- | --- | --- | --- |
| 1 | `cmake/InstallPackage.cmake` | `$<TARGET_FILE_DIR:arinc_615a_download_request_file>` wrapped in `if( TARGET … )` | That app is not in this build. The generator expression fails at *generate* time — after configure succeeds — once per entry in the runtime-dependency-set loop. |
| 2 | `scripts/build.*` | `-DCMAKE_COMPILE_WARNING_AS_ERROR=OFF` | The presets combine `/WX` (or `-Werror`) with `/external:templates-`, so warning C4127 inside Boost's own exception headers becomes a hard error while compiling the `tftp` dependency. |
| 3 | `scripts/*` | vcpkg roots pinned to `C:\vcpkg`, `C:\vb`, `C:\vp`, `C:\vi` | Under a deep path, vcpkg's libiconv build emits the `\\?\` long-path form, `cl.exe` rejects it, `iconv.lib` is dropped, and the port dies with `LNK2019`/`LNK1120` after ~40 minutes. |

The preset files themselves are **left untouched** — every fix is a command-line
override or a guarded conditional, so upstream presets can be diffed cleanly.

---

## Repository layout

```
build.bat / build.sh        one command: deps + build + run
scripts/
  doctor.*                  preflight check
  install-deps.*            toolchain + libraries
  build.*                   configure + build
  fetch-deps.*              vendor dependencies for offline builds
  _env.bat                  shared Windows environment setup
  generate-user-guide.py    regenerates the .docx
  _diagrams.py              renders the figures
app/arinc_615a_operation/   the CLI
lib/arinc_615a/             protocol library
lib/arinc_615a_commands/    command wrappers
cmake/                      presets, toolchain files, install rules
docs/                       see docs/README.md for the map
```

Root holds only what must be there: `CMakeLists.txt`, `CMakePresets.json` and
`vcpkg.json` are located by name; `.clang-format`, `.clang-tidy` and
`.editorconfig` are found by walking *up* from a source file, so moving them
silently disables them.

---

## Code style

`.clang-format` and `.clang-tidy` are in the repository root and are applied
automatically by CMake when `clang-tidy` is on `PATH`. Match the surrounding
code: this project uses a distinctive spacing style (`function( arg )`) — keep it.

Every target requires **C++23**.

---

## Line endings

`.gitattributes` pins `*.sh` to LF and `*.bat` to CRLF. Do not override this.
A shell script checked out with CRLF fails on Linux with
`bad interpreter: /usr/bin/env bash^M`, and a batch file with LF causes cmd.exe
to mis-parse multi-line constructs.

---

## Changing the documentation

| Document | How to change it |
| --- | --- |
| `README.md`, `docs/*.md` | Edit directly |
| `docs/*.docx` | **Generated.** Edit `scripts/generate-user-guide.py`, then re-run it |
| `docs/figures/*.png` | **Generated.** Edit `scripts/_diagrams.py`, then re-run the generator |

```bash
python scripts/generate-user-guide.py
```

If you add a test case to the document, mark its verification status honestly:
`VERIFIED` only if you executed it and pasted the actual output. The document's
value rests on that distinction.

---

## Testing a change

There is no unit-test target in this build — `arinc_615a_test` is an
`EXCLUDE_FROM_ALL` object library with no application linking it. Until one is
added, the acceptance set is TC-01 to TC-12 in the engineering document; all
twelve run without target hardware.

The quickest smoke test:

```bat
build.bat -c Find
```

It must print `ARINC 615A FIND Query finished` and exit `0`.

---

## Offline builds

`scripts/fetch-deps.*` vendors the five `FetchContent` dependencies into the
tree, after which configure never touches the network. See
[docs/BUILD.md](docs/BUILD.md) for the full offline procedure.

---

## Licence

MPL-2.0. Contributions are made under the same licence, and the upstream
`LICENSE` and attribution must be retained.
