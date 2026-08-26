# ARINC 615A Tool Suite — CLI

![ProjectLogo.svg](ProjectLogo.svg)

A command-line implementation of the **ARINC 615A Data Loading Protocol** — the
standard the aviation industry uses to transfer software and data between a
ground data loader and target hardware on an aircraft.

ARINC 615A defines the format and structure of the messages exchanged between
systems, the procedures for initiating and terminating transfers, and error
detection to protect data integrity. Avionics equipment uses it for:

- reporting equipment version information (part numbers, versions),
- software/data **upload** (software updates), and
- software/data **download**.

This repository is a trimmed distribution of the upstream
[ARINC 615A Tool Suite](https://git.thomas-vogt.de/thomas-vogt/arinc_615a) by
Thomas Vogt, reduced to what is needed to build and run the CLI, with added
scripts and documentation for doing so on Windows.

**Key features**

- Library handling the ARINC 615A data loader protocol, Supplements 2, 3 and 4
- Command-line application for ARINC 615A data-loading operations
- One-command dependency install and one-command build

---

## Contents

| Path | What it is |
| --- | --- |
| `app/arinc_615a_operation/` | The CLI application — `main()`, command registration, signal handling |
| `lib/arinc_615a/` | Protocol library: files, FIND, host/target state machines, TFTP glue |
| `lib/arinc_615a_commands/` | Command-registry wrappers exposing each operation to the CLI |
| `scripts/install-deps.bat` | Installs every dependency (toolchain check + vcpkg) |
| `scripts/build.bat` | Configures and builds all targets |
| `scripts/_env.bat` | Shared environment setup used by both scripts |
| [`BUILD.md`](BUILD.md) | **Detailed build documentation** — stages, dependencies, failure modes |

The upstream applications `arinc_615a_download_request_file`,
`arinc_615a_test_tha` and `arinc_615a_unit_test` are **not** included; they are
not referenced by this build.

---

## Quick start (Windows, MSVC)

```bat
scripts\install-deps.bat
scripts\build.bat release
```

Then run it:

```bat
set "PATH=C:\vi\x64-windows\bin;%PATH%"
cmake-build-msvc-static-release\app\arinc_615a_operation\arinc_615a_operation.exe --help
```

`scripts\build.bat` accepts `debug` or `release`; it defaults to `release`.

On a cold machine `install-deps.bat` takes a long while — it resolves and builds
~84 vcpkg packages, and `libiconv` alone can run for **hours** on its autotools
configure passes. Later runs take minutes, served from the vcpkg binary cache.
[`BUILD.md`](BUILD.md) walks through every stage and what it is doing.

---

## Using the CLI

`arinc_615a_operation` is command-registry driven. It takes a command via
`-c` / `--command`; each command carries its own options.

```bat
arinc_615a_operation.exe --help                REM list all commands
arinc_615a_operation.exe -c <Command> --help   REM options for one command
arinc_615a_operation.exe -c <Command> [options]
```

### ARINC 615A operations

| Command | Purpose |
| --- | --- |
| `Find` | FIND query — discover target hardware on the network |
| `Targets` | List discovered ARINC 615A targets |
| `Information` | Information operation — read part numbers and versions from a target |
| `Upload` | Upload operation (software/data to target) |
| `AdhocUpload` | Ad-hoc upload |
| `BatchUpload` | Batch upload |
| `UploadLoads` | Upload specific loads |
| `MedDownload` | Media-defined download |
| `OpDownload` | Operator-defined download |

### ARINC 665 media-set management

| Command | Purpose |
| --- | --- |
| `Create` | Create an ARINC 665 Media Set Manager |
| `ImportMediaSet` | Import a media set |
| `ImportMediaSetXml` | Import a media set from XML |
| `ListMediaSets` | List registered media sets |
| `ListLoads` | List loads across all media sets |
| `ListBatches` | List batches across all media sets |
| `RemoveMediaSet` | Remove a media set |

### A smoke test that needs no hardware

```bat
arinc_615a_operation.exe -c Find
```

Broadcasts a FIND request to `255.255.255.255`, waits out its 3-second timeout,
reports the targets found (none, on a network without avionics hardware), and
exits `0`. Everything else — `Upload`, `Information`, the download operations —
needs a real ARINC 615A target endpoint reachable on the network.

---

## Two option-parsing quirks

These are in how the command registry declares its options, not in the build.
Both will cost you time if you don't know them:

- **Use `--option=value`, not `--option value`.** Space-separated short options
  get mis-consumed: `-t 5` fails with *"the required argument for option
  '--timeout' is missing"*, while `--timeout=5` works.
- **`-l` is bound twice inside `Find`** — to both `--log-level` and
  `--targets-list`. Passing `--log-level info` makes the parser swallow the
  *next flag* as its value: *"the argument ('--timeout') for option
  '--log-level' is invalid"*. Avoid `-l` on that subcommand.

---

## Running the executable

The build links against the **dynamic** `x64-windows` vcpkg triplet, and the
presets set `VCPKG_APPLOCAL_DEPS=OFF`, so dependency DLLs are **not** copied
beside the executable. Launching it from Explorer fails with a missing-DLL error.
Put the matching vcpkg `bin` directory on `PATH` first, and match the build
configuration — do not mix release DLLs into a debug binary:

```bat
REM release build
set "PATH=C:\vi\x64-windows\bin;%PATH%"

REM debug build
set "PATH=C:\vi\x64-windows\debug\bin;%PATH%"
```

---

## Dependencies

**Toolchain** — Visual Studio 2022 C++ build tools, **CMake ≥ 4.3**, Ninja, Git.
The compiler must support **C++23**; every target sets `cxx_std_23`.

**Libraries**, installed by vcpkg from `vcpkg.json`: Boost (asio, crc, endian,
exception, hash2, multi-index, program-options, property-tree, serialization,
signals2, test), [libxml++](https://libxmlplusplus.github.io/libxmlplusplus/),
[spdlog](https://github.com/gabime/spdlog), pkgconf — about 84 packages once
resolved transitively.

**Sibling projects**, cloned automatically during configure via `FetchContent`:

- [Helper Library](https://git.thomas-vogt.de/thomas-vogt/helper)
- [ARINC 649 Tool Suite](https://git.thomas-vogt.de/thomas-vogt/arinc-649)
- [ARINC 665 Tool Suite](https://git.thomas-vogt.de/thomas-vogt/arinc_665)
- [TFTP Library](https://git.thomas-vogt.de/thomas-vogt/tftp)
- [Commands Library](https://git.thomas-vogt.de/thomas-vogt/commands)

Configure therefore **requires network access to `git.thomas-vogt.de`**; there is
no vendored copy and no offline fallback.

Full detail — including why vcpkg's working directories are pinned to short
paths like `C:\vb` and `C:\vi`, and the three failure modes that will otherwise
stop the build — is in [`BUILD.md`](BUILD.md).

---

## Other toolchains

CMake presets are also provided for **GCC**, **Clang** and a **MinGW
cross-build**, each in static/shared × debug/release, targeting Linux, Windows
MinGW and Windows MSVC. These are carried over from upstream and are not
exercised by `scripts\build.bat`, which is Windows/MSVC only.

```bash
cmake --preset gcc-static-release
cmake --build cmake-build-gcc-static-release
```

---

## References

- ARINC 615A-4 — Software Data Loader Using Ethernet Interface
- ARINC 665-5 — Loadable Software Standards
- ARINC 649 — Common Terminology and Functions for Software Distribution and Loading

---

## Protocol changes

Changes within the standards that this library accounts for.

### ARINC 615A-1
- Protocol Filenames are all uppercase
- Explicit state UDP Port 59 for data loading
- Max Value for WAIT Message is 65535 (seconds)
- Host DL shall implement _TFTP block size option_ — THA may implement _TFTP block size option_
- Transfer size option shall not be used
- Definition of block number overflow
- timeout option shall not be used
- Limit text fields to 255/ 80 characters
- Set the protocol version to _A2_
- Add exception timer to status files
- Add reference to Sorcerer's Apprentice Syndrome

### ARINC 615A-2
- Rename SNIP to FIND Protocol (FIND Identification of Network Devices)
- FIND is optional before the transfer operation
- Set the protocol version to _A3_
- Status description ignored for 0001 and 1002
- Change _LCL_ file
  - Add multiple Target Hardware (Target Hardware Code and Serial Number)
  - For Part Numbers add Amendment
- Precisely describe exception timer
- Precisely describe estimated time
- Description length can be longer than actual text (null-terminated)

### ARINC 615A-3
- Set the protocol version to _A3_
- TFTP Transfer size option is optional
- TFTP Timeout option is optional
- Add _Part Number Option_ (but copy-paste error and not usable)
- Add _Checksum Option_
- Add _Port Option_
- Add Status 0004 (in progress with status description)

### ARINC 615A-4
- _Part Number Option_ is described correctly
- _Checksum Option_ description is updated

---

## Licence

[Mozilla Public License 2.0](LICENSE). Upstream project © Thomas Vogt —
<https://git.thomas-vogt.de/thomas-vogt/arinc_615a>. The MPL requires this
licence and its attribution to be retained in redistributions, including this
one.
