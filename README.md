<div align="center">

<img src="docs/arinc-logo.webp" alt="ARINC" width="300">

# ARINC 615A Tool Suite — CLI

**Command-line implementation of the ARINC 615A Data Loading Protocol**
Discover avionics targets, read part numbers, upload and download software over Ethernet.

[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/23)
[![CMake](https://img.shields.io/badge/CMake-4.3%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)
[![Boost](https://img.shields.io/badge/Boost-1.92-F7901E?style=for-the-badge&logo=boost&logoColor=white)](https://www.boost.org/)
[![Visual Studio](https://img.shields.io/badge/MSVC-2022-5C2D91?style=for-the-badge&logo=visualstudio&logoColor=white)](https://visualstudio.microsoft.com/)

[![Windows](https://img.shields.io/badge/Windows-x64-0078D6?style=for-the-badge&logo=windows&logoColor=white)](#quick-start--one-command)
[![Linux](https://img.shields.io/badge/Linux-x64%20%7C%20arm64-FCC624?style=for-the-badge&logo=linux&logoColor=black)](#quick-start--one-command)
[![vcpkg](https://img.shields.io/badge/vcpkg-manifest-1E90FF?style=for-the-badge&logo=microsoft&logoColor=white)](https://vcpkg.io/)
[![Licence](https://img.shields.io/badge/Licence-MPL--2.0-A6CE39?style=for-the-badge&logo=mozilla&logoColor=white)](LICENSE)
[![Protocol](https://img.shields.io/badge/ARINC-615A--4-1F6FEB?style=for-the-badge&logo=airbus&logoColor=white)](#protocol-background)

</div>

---

## What this is

ARINC 615A is the standard the aviation industry uses to move software and data
between a ground **data loader** and **target hardware** on an aircraft — the
LRUs, computers and controllers that need software updates. It defines the
message formats, the transfer procedures, and the error detection that protects
data integrity.

This repository builds `arinc_615a_operation`, a single command-line tool that
speaks that protocol. It can:

- **discover** targets on the network (FIND),
- **interrogate** them for part numbers and versions,
- **upload** software and data to them,
- **download** software and data from them,
- and manage the **ARINC 665 media sets** the software is packaged in.

It is a trimmed distribution of the upstream
[ARINC 615A Tool Suite](https://git.thomas-vogt.de/thomas-vogt/arinc_615a) by
Thomas Vogt, reduced to what is needed to build and run the CLI, with added
setup scripts and documentation.

---

## What it does — block diagram

```mermaid
flowchart TB
    subgraph GROUND["🖥️  Ground side — this tool"]
        direction TB
        CLI["<b>arinc_615a_operation</b><br/>command-line entry point"]
        REG["Command Registry<br/><i>16 commands</i>"]
        CMD615["arinc_615a_commands<br/><i>operations · targets</i>"]
        CMD665["arinc_665_commands<br/><i>media set manager</i>"]
        HOST["Host protocol<br/><i>state machines</i>"]
        FILES["Protocol files<br/><i>LCI · LCL · LUS · LNR …</i>"]
        FIND["FIND client<br/><i>discovery</i>"]
        TFTP["TFTP client / server<br/><i>+ 615A options</i>"]
    end

    subgraph AIR["✈️  Aircraft side — target hardware"]
        THA["THA<br/>Target Hardware Application"]
        LRU["Avionics LRU<br/><i>software · part numbers</i>"]
    end

    CLI --> REG
    REG --> CMD615
    REG --> CMD665
    CMD615 --> HOST
    CMD615 --> FIND
    HOST --> FILES
    FILES --> TFTP
    TFTP <-->|"UDP · file transfer"| THA
    FIND <-->|"UDP broadcast · IRQ/IAN"| THA
    THA --> LRU
    CMD665 -.->|"loads to upload"| HOST

    classDef ground fill:#1F6FEB,stroke:#0D419D,color:#fff
    classDef air fill:#238636,stroke:#116329,color:#fff
    classDef wire fill:#8250DF,stroke:#5A32A3,color:#fff
    class CLI,REG,CMD615,CMD665,HOST,FILES ground
    class THA,LRU air
    class TFTP,FIND wire
```

The tool is the **host** (ground data loader). The box on the aircraft is the
**target**. Everything on the data path is a *file* moved over TFTP — ARINC 615A
is file-driven, not message-driven. FIND is the one exception: a plain UDP
request/answer pair used to discover what is out there before a transfer.

---

## What really happens when you run it

```mermaid
sequenceDiagram
    autonumber
    participant U as You
    participant M as main()
    participant R as CommandRegistry
    participant O as Host operation
    participant T as Target (THA)

    U->>M: -c Information --target-address=10.0.0.5
    M->>M: set log levels · create io_context
    M->>M: install SIGINT/SIGTERM handlers
    M->>R: register 615A + 665 commands
    M->>R: dispatch("Information")
    R->>O: build + start operation

    O->>T: TFTP write LCI (initialisation)
    T-->>O: initialisation response

    loop until terminating status
        T->>O: LCS (status · progress · estimated time)
        Note over O,T: exception timer restarts on every status
    end

    T->>O: LCL (part numbers · target hardware ids)
    O-->>U: report result
    M->>M: cancel signals · stop io_context · join
```

**Two details that matter operationally.** The **exception timer** is a
watchdog: if the target goes quiet for longer than the negotiated interval, the
host fails the operation instead of hanging forever. And **the first `Ctrl-C` is
a graceful abort**, not a kill — it sends a protocol abort so the target is not
left mid-load in an undefined state. A second `Ctrl-C` terminates hard.

---

## Setup — what the scripts do

Two paths, one command each. **Windows** gets its libraries from vcpkg;
**Linux** gets them from the distro, which is why it is minutes rather than hours.

```mermaid
flowchart LR
    W(["build.bat"]) --> WD["scripts\install-deps.bat<br/><i>vcpkg · ~84 packages</i>"]
    WD --> WB["scripts\build.bat<br/><i>msvc-static-release</i>"]
    WB --> WR["run with vcpkg<br/>DLLs on PATH"]

    L(["./build.sh"]) --> LD["scripts/install-deps.sh<br/><i>apt · dnf · pacman · zypper</i>"]
    LD --> LB["scripts/build.sh<br/><i>gcc-static-release</i>"]
    LB --> LR["run directly<br/><i>system libraries</i>"]

    classDef win fill:#0078D6,stroke:#005A9E,color:#fff
    classDef lin fill:#8A6D00,stroke:#5E4A00,color:#fff
    class W,WD,WB,WR win
    class L,LD,LB,LR lin
```

The Windows path in detail — the two hazards marked are the ones that will
otherwise cost you an afternoon:

```mermaid
flowchart TD
    START(["scripts\install-deps.bat"]) --> VS{"Visual Studio<br/>C++ toolset?"}
    VS -->|missing| VSFIX["print winget command<br/>and stop"]
    VS -->|found| VCVARS["call vcvars64.bat"]
    VCVARS --> WARN["⚠ re-assert VCPKG_ROOT<br/><i>vcvars overwrites it</i>"]
    WARN --> CM{"CMake ≥ 4.3?"}
    CM -->|"only VS's 3.31"| WINGET["winget install Kitware.CMake"]
    CM -->|found| NINJA
    WINGET --> NINJA["Ninja<br/><i>from PATH or VS</i>"]
    NINJA --> CLONE["clone + bootstrap vcpkg<br/><b>C:\vcpkg</b>"]
    CLONE --> INSTALL["vcpkg install from vcpkg.json<br/>~84 packages"]
    INSTALL --> SHORT["🔑 short roots<br/><b>C:\vb · C:\vp · C:\vi</b>"]
    SHORT --> DONE1(["dependencies ready"])

    DONE1 --> BUILD(["scripts\build.bat release"])
    BUILD --> CONF["cmake --preset msvc-static-release"]
    CONF --> FETCH["FetchContent clones 5 repos<br/><i>helper · arinc_649 · arinc_665<br/>tftp · commands</i>"]
    FETCH --> GEN["generate build.ninja"]
    GEN --> COMPILE["ninja — compile all targets"]
    COMPILE --> EXE(["<b>arinc_615a_operation.exe</b>"])

    classDef ok fill:#238636,stroke:#116329,color:#fff
    classDef warn fill:#9E6A03,stroke:#7D4E00,color:#fff
    classDef bad fill:#DA3633,stroke:#A40E26,color:#fff
    class DONE1,EXE ok
    class WARN,SHORT warn
    class VSFIX bad
```

### Why `C:\vb`, `C:\vp`, `C:\vi`

Not cosmetic. vcpkg builds `libiconv` with autotools, and during `make install`
it forms a path by concatenating the package directory **with the full install
prefix**. Under a normal project location that blows past Windows' 260-character
limit, libtool falls back to the `\\?\` long-path form, and `cl.exe` rejects it:

```
cl : Command line warning D9002 : ignoring unknown option '//?/C:/…/iconv.lib'
iconv.obj : error LNK2019: unresolved external symbol libiconv_open
.libs\iconv.exe : fatal error LNK1120: 7 unresolved externals
```

The import library is silently dropped and the port dies — after about 40
minutes of work. Short roots avoid it entirely. Full detail in
[docs/BUILD.md](docs/BUILD.md).

---

## Quick start — one command

From a fresh clone, this installs every dependency, builds, and runs the tool:

**Windows**

```bat
build.bat
```

**Linux**

```bash
./build.sh
```

That is the whole thing. Arguments pass straight through, so you can build and
run something specific in the same breath:

```bat
build.bat -c Find                REM discover targets on the network
build.bat debug -c Find          REM as a debug build
build.bat --no-run               REM install and build only
```

```bash
./build.sh -c Find
./build.sh debug -c Find
./build.sh --no-run
```

> **First run is slow.** On Windows it builds ~84 vcpkg packages and `libiconv`
> alone can run for hours on its two autotools configure passes. On Linux it
> installs distro packages, which takes minutes. Later runs take seconds. Prefer
> not to wait on Windows? Grab a prebuilt binary from [Releases](../../releases).

### Or run the steps separately

| | Windows | Linux |
| --- | --- | --- |
| Dependencies | `scripts\install-deps.bat` | `scripts/install-deps.sh` |
| Build | `scripts\build.bat release` | `scripts/build.sh release` |

Both accept `debug` or `release`, defaulting to `release`.

---

## Using the CLI

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
| `Information` | Read part numbers and versions from a target |
| `Upload` | Upload software/data to a target |
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

### Smoke test — no hardware needed

```bat
arinc_615a_operation.exe -c Find
```

Broadcasts a FIND request to `255.255.255.255`, waits out its 3-second timeout,
reports what it found (nothing, on a network without avionics hardware), exits `0`.

Every other operation needs a reachable ARINC 615A target.

---

## ⚠ Two option-parsing quirks

These are in how the command registry declares options. Both cost time if you
don't know them:

| Symptom | Cause | Do this |
| --- | --- | --- |
| `the required argument for option '--timeout' is missing` | Space-separated short options get mis-consumed | Use `--timeout=5`, not `-t 5` |
| `the argument ('--timeout') for option '--log-level' is invalid` | `-l` is bound to **both** `--log-level` and `--targets-list` inside `Find` | Avoid `-l` on that subcommand |

## ⚠ On Windows the executable needs DLLs on PATH

`build.bat` handles this for you. Running the exe **directly** does not.

The Windows build links against the **dynamic** `x64-windows` triplet with
`VCPKG_APPLOCAL_DEPS=OFF`, so dependency DLLs are **not** copied beside the exe.
Double-clicking it in Explorer fails with a missing-DLL error. Match the
configuration — never mix release DLLs into a debug binary:

```bat
set "PATH=C:\vi\x64-windows\bin;%PATH%"        REM release
set "PATH=C:\vi\x64-windows\debug\bin;%PATH%"  REM debug
```

**Linux has no such problem** — it links against system libraries already on the
loader search path:

```bash
cmake-build-gcc-static-release/app/arinc_615a_operation/arinc_615a_operation -c Find
```

The [prebuilt Windows binary](../../releases) is bundled with its DLLs and needs
no `PATH` setup either.

---

## Repository layout

```
.
├── app/arinc_615a_operation/   CLI application — main(), signals, wiring
├── lib/
│   ├── arinc_615a/             protocol library
│   │   ├── host/               ground-loader state machines
│   │   ├── target/             target-hardware state machines
│   │   ├── files/              LCI · LCL · LUS · LNR … encode/decode
│   │   ├── find/               discovery: packets · clients · servers
│   │   ├── tftp/               615A-specific TFTP options
│   │   └── information/        shared value types
│   └── arinc_615a_commands/    CLI command wrappers
├── cmake/                      toolchain files, presets, install rules
├── scripts/                    install-deps · build   (.bat Windows, .sh Linux)
├── docs/                       BUILD.md · ARCHITECTURE.md · CODE-TRACE.md
├── build.bat                   one command: deps + build + run  (Windows)
└── build.sh                    one command: deps + build + run  (Linux)
```

---

## Documentation

| Document | What's in it |
| --- | --- |
| **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)** | How the codebase works — layer map, directory-by-directory walkthrough, protocol file table, operation flow, design conventions |
| **[docs/CODE-TRACE.md](docs/CODE-TRACE.md)** | Function-by-function trace from `main()` to the wire and back through the handler callbacks, every entry carrying its `file:line`. 23 sections covering concurrency, dispatch, each protocol operation, the file codec, the TFTP shim, timers and abort, status codes, customisation points and known defects |
| **[docs/BUILD.md](docs/BUILD.md)** | Every build stage in order, all dependencies and how to install them, the failure modes that will otherwise stop you, and every linked resource in one place |

---

## Dependencies

The setup scripts install all of this. It's listed so you know what lands on
your machine.

| | Windows | Linux |
| --- | --- | --- |
| Compiler | Visual Studio 2022 C++ build tools | GCC 13+ |
| Build system | **CMake ≥ 4.3** + Ninja | **CMake ≥ 4.3** + Ninja |
| Libraries from | **vcpkg**, via `vcpkg.json` (~84 packages) | **distro packages** — apt · dnf · pacman · zypper |
| Installed to | `C:\vi` (plus `C:\vb`, `C:\vp` as vcpkg scratch) | system prefix |
| DLL/so handling | needs `PATH` set — see above | nothing to do |

The compiler must support **C++23**; every target sets `cxx_std_23`.

Most distributions ship a CMake older than 4.3, so `scripts/install-deps.sh`
fetches the official Kitware binary into a git-ignored `.toolchain/` when needed.

**Libraries** — Boost (asio, crc, endian, exception, hash2, multi-index,
program-options, property-tree, serialization, signals2, test),
[libxml++](https://libxmlplusplus.github.io/libxmlplusplus/),
[spdlog](https://github.com/gabime/spdlog),
[fmt](https://fmt.dev/), pkgconf. `libxml++` is required by the `arinc_665`
dependency rather than by this repository's own libraries.

**Sibling projects**, cloned during configure via `FetchContent`:
[helper](https://git.thomas-vogt.de/thomas-vogt/helper) ·
[arinc_649](https://git.thomas-vogt.de/thomas-vogt/arinc-649) ·
[arinc_665](https://git.thomas-vogt.de/thomas-vogt/arinc_665) ·
[tftp](https://git.thomas-vogt.de/thomas-vogt/tftp) ·
[commands](https://git.thomas-vogt.de/thomas-vogt/commands)

> Configure **requires network access to `git.thomas-vogt.de`**. There is no
> vendored copy and no offline fallback.

### Other toolchains

`build.bat` drives the **MSVC** presets and `build.sh` drives the **GCC** ones.
Presets also exist for **Clang** and a **MinGW cross-build** (Windows binaries
from Linux), each in static/shared × debug/release. Those two come from upstream
and are not exercised by either script:

```bash
cmake --preset clang-static-release
cmake --build cmake-build-clang-static-release
```

> **The Linux scripts have not been run.** They were written against the presets
> and distro package names on a Windows machine with no Linux environment
> available, and are syntax-checked only. Package names are the most likely
> thing to need adjusting across distribution releases. See
> [docs/BUILD.md §7](docs/BUILD.md) for the detail.

---

## Protocol background

This library implements **Supplements 2, 3 and 4**. Selected changes it accounts
for:

| Supplement | Notable changes |
| --- | --- |
| **615A-1** | Uppercase protocol filenames · UDP port 59 · block-size option mandatory for host · transfer-size and timeout options forbidden · exception timer added to status files |
| **615A-2** | SNIP renamed **FIND** and made optional before transfer · protocol version `A3` · `LCL` gains multiple target hardware and part-number amendments · exception timer and estimated time defined precisely |
| **615A-3** | Transfer-size and timeout options optional · **checksum** and **port** options added · status `0004` (in progress with description) · part-number option added but unusable as written |
| **615A-4** | Part-number option corrected · checksum option description updated |

**References** — ARINC 615A-4 (Software Data Loader Using Ethernet Interface),
ARINC 665-5 (Loadable Software Standards), ARINC 649 (Common Terminology and
Functions for Software Distribution and Loading).

---

## Licence

[![Licence](https://img.shields.io/badge/Licence-MPL--2.0-A6CE39?style=flat-square&logo=mozilla&logoColor=white)](LICENSE)

Mozilla Public License 2.0. Upstream project © Thomas Vogt —
<https://git.thomas-vogt.de/thomas-vogt/arinc_615a>. The MPL requires this
licence and its attribution to be retained in redistributions, including this one.

ARINC® is a trademark of its respective owner. This project implements the
publicly documented ARINC 615A protocol and is **not affiliated with, endorsed
by, or a product of ARINC**. The ARINC standards themselves are published by
SAE ITC and are not redistributed here.
