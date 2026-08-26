#!/usr/bin/env python3
"""Render the diagrams used by the installation and test procedure document.

Pure matplotlib - no external drawing tools. Each function returns the path of
a PNG written into the given output directory.
"""
import pathlib

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import FancyArrowPatch, FancyBboxPatch

DPI = 200

BLUE = "#0B5CA8"
BLUE_L = "#E4EEF8"
GREEN = "#1F7A3D"
GREEN_L = "#E7F5EC"
AMBER = "#9E6A03"
AMBER_L = "#FDF3E2"
RED = "#B3261E"
RED_L = "#FBE9E7"
GREY = "#5A6472"
GREY_L = "#EEF1F5"
INK = "#1A1A1A"


def _fig(w, h):
    fig, ax = plt.subplots(figsize=(w, h))
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.axis("off")
    fig.patch.set_facecolor("white")
    return fig, ax


def _box(ax, x, y, w, h, text, fc=BLUE_L, ec=BLUE, fs=8, bold=False, tc=INK):
    ax.add_patch(FancyBboxPatch((x, y), w, h,
                                boxstyle="round,pad=0.6,rounding_size=1.5",
                                linewidth=1.2, facecolor=fc, edgecolor=ec))
    ax.text(x + w / 2, y + h / 2, text, ha="center", va="center",
            fontsize=fs, color=tc, fontweight="bold" if bold else "normal",
            linespacing=1.45)


def _arrow(ax, x1, y1, x2, y2, color=GREY, style="-|>", lw=1.3, ls="-"):
    ax.add_patch(FancyArrowPatch((x1, y1), (x2, y2), arrowstyle=style,
                                 mutation_scale=11, linewidth=lw,
                                 color=color, linestyle=ls,
                                 shrinkA=2, shrinkB=2))


def _label(ax, x, y, text, fs=7, color=GREY, ha="center", style="normal"):
    ax.text(x, y, text, ha=ha, va="center", fontsize=fs, color=color, style=style)


def _save(fig, out, name):
    p = pathlib.Path(out) / name
    fig.savefig(p, dpi=DPI, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    return p


# --------------------------------------------------------------- diagrams --
def system_context(out):
    fig, ax = _fig(9, 4.4)
    ax.add_patch(FancyBboxPatch((1, 8), 44, 84, boxstyle="round,pad=0.6,rounding_size=2",
                                linewidth=1.1, facecolor="#F7F9FC", edgecolor="#C7D2E0", linestyle="--"))
    ax.add_patch(FancyBboxPatch((60, 8), 38, 84, boxstyle="round,pad=0.6,rounding_size=2",
                                linewidth=1.1, facecolor="#F4FAF6", edgecolor="#BFE0C9", linestyle="--"))
    _label(ax, 23, 95, "GROUND SIDE  —  this tool", fs=9, color=BLUE)
    _label(ax, 79, 95, "AIRCRAFT SIDE  —  target hardware", fs=9, color=GREEN)

    _box(ax, 6, 76, 34, 11, "arinc_615a_operation\ncommand-line entry point", bold=True)
    _box(ax, 6, 61, 34, 10, "Command Registry\n16 commands")
    _box(ax, 6, 46, 16, 10, "615A\ncommands", fs=7.5)
    _box(ax, 24, 46, 16, 10, "665 media\ncommands", fs=7.5)
    _box(ax, 6, 31, 34, 10, "Host protocol state machines")
    _box(ax, 6, 16, 34, 10, "Protocol files\nLCI  LCL  LCS  LUS  LNR", fc=GREY_L, ec=GREY)

    _box(ax, 46, 44, 12, 14, "TFTP\n+ 615A\noptions", fc=AMBER_L, ec=AMBER, fs=7.5)
    _box(ax, 46, 64, 12, 12, "FIND\nclient", fc=AMBER_L, ec=AMBER, fs=7.5)

    _box(ax, 66, 56, 26, 12, "THA\nTarget Hardware Application", fc=GREEN_L, ec=GREEN, bold=True)
    _box(ax, 66, 30, 26, 12, "Avionics LRU\nsoftware  ·  part numbers", fc=GREEN_L, ec=GREEN)

    for y1, y2 in ((76, 71), (61, 56), (46, 41), (31, 26)):
        _arrow(ax, 23, y1, 23, y2)
    _arrow(ax, 40, 21, 46, 48)
    _arrow(ax, 40, 66, 46, 70)
    _arrow(ax, 58, 51, 66, 58, color=AMBER, style="<|-|>")
    _arrow(ax, 58, 70, 66, 64, color=AMBER, style="<|-|>")
    _arrow(ax, 79, 56, 79, 42, color=GREEN)
    _label(ax, 62, 45, "UDP\nfiles", fs=6.5, color=AMBER)
    _label(ax, 62, 76, "UDP\nbroadcast", fs=6.5, color=AMBER)
    return _save(fig, out, "fig01-system-context.png")


def layer_stack(out):
    fig, ax = _fig(8, 4.2)
    rows = [
        ("CLI executable", "app/arinc_615a_operation - main(), signals", BLUE_L, BLUE),
        ("Command layer", "lib/arinc_615a_commands - options, console output", BLUE_L, BLUE),
        ("Host protocol", "lib/arinc_615a/host - Protocol, OperationImpl", BLUE_L, BLUE),
        ("Protocol files", "lib/arinc_615a/files - encode / decode", GREY_L, GREY),
        ("615A TFTP shim", "lib/arinc_615a/tftp - options, retries", AMBER_L, AMBER),
        ("External", "tftp  arinc_665  arinc_649  Boost.Asio  spdlog", GREY_L, GREY),
    ]
    y = 84
    for name, detail, fc, ec in rows:
        _box(ax, 12, y, 62, 11, "", fc=fc, ec=ec)
        ax.text(15, y + 7.0, name, ha="left", va="center", fontsize=8.5, fontweight="bold", color=INK)
        ax.text(15, y + 3.2, detail, ha="left", va="center", fontsize=7, color=GREY)
        y -= 14
    _arrow(ax, 8, 92, 8, 6, color=BLUE, lw=1.6)
    _label(ax, 5.0, 49, "calls descend", fs=7, color=BLUE)
    _arrow(ax, 78, 6, 78, 92, color=GREEN, lw=1.6)
    _label(ax, 84, 49, "results return only\nvia handlers bound\nat setup time", fs=6.8, color=GREEN)
    return _save(fig, out, "fig02-layer-stack.png")


def install_windows(out):
    fig, ax = _fig(8.4, 4.6)
    steps = [
        (78, "build.bat", GREEN_L, GREEN, True),
        (65, "Visual Studio C++ toolset present?", BLUE_L, BLUE, False),
        (52, "call vcvars64.bat\nthen RE-ASSERT VCPKG_ROOT", AMBER_L, AMBER, False),
        (39, "CMake >= 4.3?   (VS ships 3.31)", BLUE_L, BLUE, False),
        (26, "clone + bootstrap vcpkg   C:\\vcpkg", BLUE_L, BLUE, False),
        (13, "vcpkg install - approx. 84 packages\nshort roots  C:\\vb  C:\\vp  C:\\vi", AMBER_L, AMBER, False),
    ]
    for y, text, fc, ec, bold in steps:
        _box(ax, 14, y, 56, 10, text, fc=fc, ec=ec, bold=bold, fs=8)
    for y in (78, 65, 52, 39, 26):
        _arrow(ax, 42, y, 42, y - 3)
    _box(ax, 76, 65, 22, 10, "stop:\nprint winget\ncommand", fc=RED_L, ec=RED, fs=7)
    _arrow(ax, 70, 70, 76, 70, color=RED)
    _label(ax, 73, 73, "no", fs=6.5, color=RED)
    _box(ax, 76, 39, 22, 10, "winget install\nKitware.CMake", fc=AMBER_L, ec=AMBER, fs=7)
    _arrow(ax, 70, 44, 76, 44, color=AMBER)
    _label(ax, 73, 47, "old", fs=6.5, color=AMBER)
    _arrow(ax, 42, 13, 42, 8)
    _box(ax, 14, -2, 56, 9, "cmake configure  ->  ninja build  ->  run", fc=GREEN_L, ec=GREEN, bold=True, fs=8)
    _label(ax, 42, 92, "Windows installation flow", fs=9.5, color=BLUE)
    return _save(fig, out, "fig03-install-windows.png")


def install_linux(out):
    fig, ax = _fig(8.4, 3.6)
    steps = [
        (80, "./build.sh", GREEN_L, GREEN, True),
        (65, "detect package manager\napt  ·  dnf  ·  pacman  ·  zypper", BLUE_L, BLUE, False),
        (50, "install g++, ninja, git, pkg-config,\nboost, spdlog, fmt, libxml++", BLUE_L, BLUE, False),
        (35, "CMake >= 4.3?", BLUE_L, BLUE, False),
        (20, "cmake configure (gcc-static-release)\nninja build  ->  run", GREEN_L, GREEN, True),
    ]
    for y, text, fc, ec, bold in steps:
        _box(ax, 14, y, 56, 11, text, fc=fc, ec=ec, bold=bold, fs=8)
    for y in (80, 65, 50, 35):
        _arrow(ax, 42, y, 42, y - 4)
    _box(ax, 76, 35, 22, 11, "fetch Kitware\nbinary into\n.toolchain/", fc=AMBER_L, ec=AMBER, fs=7)
    _arrow(ax, 70, 40, 76, 40, color=AMBER)
    _label(ax, 73, 44, "too old", fs=6.5, color=AMBER)
    _label(ax, 42, 95, "Linux installation flow  —  system libraries, no vcpkg", fs=9.5, color=BLUE)
    return _save(fig, out, "fig04-install-linux.png")


def find_sequence(out):
    fig, ax = _fig(8, 3.2)
    _box(ax, 6, 82, 26, 10, "Host  (this tool)", fc=BLUE_L, ec=BLUE, bold=True, fs=8.5)
    _box(ax, 64, 82, 30, 10, "Targets on the network", fc=GREEN_L, ec=GREEN, bold=True, fs=8.5)
    ax.plot([19, 19], [10, 80], color=BLUE, lw=1, ls="--")
    ax.plot([79, 79], [10, 80], color=GREEN, lw=1, ls="--")

    _arrow(ax, 19, 68, 79, 68, color=AMBER, lw=1.5)
    _label(ax, 49, 72, "UDP broadcast 255.255.255.255\nInformation Request  (IRQ, opcode 1)", fs=7, color=AMBER)
    _arrow(ax, 79, 52, 19, 52, color=GREEN, lw=1.3)
    _label(ax, 49, 56, "Information Answer  (IAN, opcode 2)   -  one per target", fs=7, color=GREEN)
    _arrow(ax, 79, 42, 19, 42, color=GREEN, lw=1.3, ls=":")
    _label(ax, 49, 46, "further answers until the timeout expires", fs=7, color=GREY)

    ax.add_patch(FancyBboxPatch((8, 20), 22, 12, boxstyle="round,pad=0.5,rounding_size=1.5",
                                linewidth=1.1, facecolor=GREY_L, edgecolor=GREY))
    ax.text(19, 26, "--timeout\ndefault 3 s", ha="center", va="center", fontsize=7, color=INK)
    _label(ax, 49, 12, "With no hardware present, zero answers arrive and the tool exits 0.\n"
                       "That is a PASS: it proves the stack runs and terminates.", fs=7, color=GREY)
    return _save(fig, out, "fig05-find-sequence.png")


def information_sequence(out):
    fig, ax = _fig(8, 4.0)
    _box(ax, 6, 86, 26, 9, "Host  (this tool)", fc=BLUE_L, ec=BLUE, bold=True, fs=8.5)
    _box(ax, 66, 86, 28, 9, "Target  (THA)", fc=GREEN_L, ec=GREEN, bold=True, fs=8.5)
    ax.plot([19, 19], [8, 84], color=BLUE, lw=1, ls="--")
    ax.plot([80, 80], [8, 84], color=GREEN, lw=1, ls="--")

    _arrow(ax, 19, 76, 80, 76, color=BLUE, lw=1.4)
    _label(ax, 49, 79.5, "TFTP write  LCI   Load Configuration Initialisation", fs=7, color=BLUE)
    _arrow(ax, 80, 66, 19, 66, color=GREEN, lw=1.3)
    _label(ax, 49, 69.5, "initialisation response", fs=7, color=GREEN)

    ax.add_patch(FancyBboxPatch((12, 34), 76, 26, boxstyle="round,pad=0.5,rounding_size=1.5",
                                linewidth=1.1, facecolor="#FBFCFD", edgecolor="#C7D2E0", linestyle="--"))
    _label(ax, 22, 57, "loop", fs=7, color=GREY)
    _arrow(ax, 80, 50, 19, 50, color=GREEN, lw=1.3)
    _label(ax, 49, 53.5, "LCS   status  ·  progress ratio  ·  estimated time", fs=7, color=GREEN)
    _label(ax, 49, 41, "exception timer restarts on every status file\n"
                       "no status within --dlp-timeout (13 s)  ->  operation fails",
           fs=6.8, color=AMBER)

    _arrow(ax, 80, 24, 19, 24, color=GREEN, lw=1.5)
    _label(ax, 49, 27.5, "LCL   Load Configuration List  -  part numbers, THW IDs", fs=7, color=GREEN)
    _box(ax, 6, 10, 30, 9, "print result, exit 0", fc=GREEN_L, ec=GREEN, fs=7.5)
    return _save(fig, out, "fig06-information-sequence.png")


def upload_sequence(out):
    fig, ax = _fig(8, 3.6)
    _box(ax, 6, 86, 26, 9, "Host  (file server)", fc=BLUE_L, ec=BLUE, bold=True, fs=8.5)
    _box(ax, 66, 86, 28, 9, "Target  (THA)", fc=GREEN_L, ec=GREEN, bold=True, fs=8.5)
    ax.plot([19, 19], [6, 84], color=BLUE, lw=1, ls="--")
    ax.plot([80, 80], [6, 84], color=GREEN, lw=1, ls="--")

    _arrow(ax, 19, 76, 80, 76, color=BLUE, lw=1.4)
    _label(ax, 49, 79.5, "LUI   Upload Initialisation", fs=7, color=BLUE)
    _arrow(ax, 80, 64, 19, 64, color=GREEN, lw=1.3)
    _label(ax, 49, 67.5, "LUR   Upload Request  -  target asks for each file", fs=7, color=GREEN)
    _arrow(ax, 19, 52, 80, 52, color=BLUE, lw=1.4)
    _label(ax, 49, 55.5, "TFTP read  -  host serves the load files", fs=7, color=BLUE)
    _arrow(ax, 80, 40, 19, 40, color=GREEN, lw=1.3, ls=":")
    _label(ax, 49, 43.5, "LUS   Upload Status  -  repeated until terminating status", fs=7, color=GREEN)
    _label(ax, 49, 22, "The host acts as a TFTP SERVER during upload: the target pulls the files.\n"
                       "Loads come from the Media Set Manager (--media-set-manager-dir).",
           fs=7, color=GREY)
    _box(ax, 6, 6, 34, 9, "verify with a follow-up\nInformation operation", fc=AMBER_L, ec=AMBER, fs=7)
    return _save(fig, out, "fig07-upload-sequence.png")


def abort_model(out):
    fig, ax = _fig(8, 2.9)
    _box(ax, 4, 62, 26, 14, "Operation running", fc=BLUE_L, ec=BLUE, bold=True, fs=8)
    _box(ax, 38, 62, 26, 14, "Graceful abort\nsent to target", fc=AMBER_L, ec=AMBER, bold=True, fs=8)
    _box(ax, 72, 62, 24, 14, "Hard terminate", fc=RED_L, ec=RED, bold=True, fs=8)
    _arrow(ax, 30, 69, 38, 69, color=AMBER, lw=1.5)
    _label(ax, 34, 80, "1st Ctrl-C", fs=7, color=AMBER)
    _arrow(ax, 64, 69, 72, 69, color=RED, lw=1.5)
    _label(ax, 68, 80, "2nd Ctrl-C", fs=7, color=RED)

    _box(ax, 4, 34, 26, 12, "target told to stop;\nstate stays defined", fc=GREEN_L, ec=GREEN, fs=7)
    _arrow(ax, 51, 62, 24, 46, color=GREY, ls=":")
    _box(ax, 60, 34, 36, 12, "target may be left mid-load\nin an undefined state", fc=RED_L, ec=RED, fs=7)
    _arrow(ax, 84, 62, 80, 46, color=GREY, ls=":")
    _label(ax, 50, 16, "On real hardware, allow the first abort to complete before pressing again.",
           fs=7.5, color=INK)
    _label(ax, 50, 6, "Separately: --dlp-timeout (default 13 s) fails a stalled operation automatically.",
           fs=7, color=AMBER)
    return _save(fig, out, "fig08-abort-model.png")


def test_flow(out):
    fig, ax = _fig(8.4, 3.4)
    groups = [
        (3, "TC-01..03\ntoolchain,\nbuild, artefact", BLUE_L, BLUE),
        (23, "TC-04..05\ncommand\ncatalogue", BLUE_L, BLUE),
        (43, "TC-06\nFIND smoke\ntest", GREEN_L, GREEN),
        (63, "TC-07..08\nmedia set\nstore", GREEN_L, GREEN),
        (83, "TC-09..12\nnegative +\npackaging", AMBER_L, AMBER),
    ]
    for x, text, fc, ec in groups:
        _box(ax, x, 55, 16, 22, text, fc=fc, ec=ec, fs=7.5)
    for x in (19, 39, 59, 79):
        _arrow(ax, x, 66, x + 4, 66)
    _box(ax, 20, 26, 60, 14, "ACCEPTANCE:  TC-01 to TC-12 all pass\nno target hardware required",
         fc=GREEN_L, ec=GREEN, bold=True, fs=8.5)
    _arrow(ax, 50, 55, 50, 40, color=GREEN, lw=1.6)
    _box(ax, 8, 4, 38, 13, "TC-13..15  operational set\nrequires ARINC 615A hardware",
         fc=GREY_L, ec=GREY, fs=7.5)
    _box(ax, 54, 4, 38, 13, "TC-16  Linux build\nrequires a Linux host", fc=GREY_L, ec=GREY, fs=7.5)
    _arrow(ax, 30, 26, 27, 17, color=GREY, ls=":")
    _arrow(ax, 70, 26, 73, 17, color=GREY, ls=":")
    return _save(fig, out, "fig09-test-flow.png")


def install_layout(out):
    fig, ax = _fig(8, 3.2)
    _box(ax, 4, 74, 42, 16, "C:\\vcpkg\nvcpkg clone", fc=GREY_L, ec=GREY, fs=8)
    _box(ax, 4, 54, 42, 16, "C:\\vb   build trees\nC:\\vp   packages", fc=AMBER_L, ec=AMBER, fs=8)
    _box(ax, 4, 34, 42, 16, "C:\\vi\ninstalled headers, libs, DLLs", fc=BLUE_L, ec=BLUE, fs=8)
    _box(ax, 54, 54, 44, 36,
         "repository/\n  cmake-build-msvc-static-release/\n    app/arinc_615a_operation/\n"
         "      arinc_615a_operation.exe", fc=GREEN_L, ec=GREEN, fs=7.5)
    _arrow(ax, 46, 42, 54, 62, color=BLUE)
    _label(ax, 50, 30, "links against", fs=6.5, color=BLUE)
    _box(ax, 4, 8, 94, 18,
         "Windows runtime requirement:  the DLLs in C:\\vi\\x64-windows\\bin are NOT copied\n"
         "beside the executable (VCPKG_APPLOCAL_DEPS=OFF).  Put that directory on PATH,\n"
         "matching debug or release.   build.bat does this for you.   Linux needs nothing.",
         fc=AMBER_L, ec=AMBER, fs=8)
    return _save(fig, out, "fig10-install-layout.png")


def osi_model(out):
    """ARINC 615A mapped onto the OSI reference model, with the real port numbers."""
    fig, ax = _fig(9, 5.2)
    rows = [
        (7, "Application", "ARINC 615A Data Load Protocol  ·  FIND discovery",
         "protocol files LCI LCL LCS LUI LUR LUS LNR LNA LNS", BLUE_L, BLUE),
        (6, "Presentation", "ARINC 615A file encoding  ·  ARINC 665 load format",
         "binary records, length-prefixed null-terminated strings", BLUE_L, BLUE),
        (5, "Session", "DLP operation session  ·  TFTP transfer session",
         "exception timer 13 s  ·  DLP retries", BLUE_L, BLUE),
        (4, "Transport", "UDP  —  connectionless",
         "data load port 59   ·   FIND port 1001   ·   TFTP options RFC 2347-2349", AMBER_L, AMBER),
        (3, "Network", "IPv4   (ARP for address resolution, ICMP for diagnostics)",
         "unicast to target  ·  255.255.255.255 broadcast for FIND", GREY_L, GREY),
        (2, "Data link", "Ethernet  IEEE 802.3   —   MAC framing",
         "broadcast domain must include host and target", GREY_L, GREY),
        (1, "Physical", "10/100/1000BASE-T aircraft data-load network",
         "typically a dedicated maintenance port", GREY_L, GREY),
    ]
    y = 84
    for num, name, main, detail, fc, ec in rows:
        _box(ax, 10, y, 84, 11.5, "", fc=fc, ec=ec)
        ax.text(13, y + 5.8, str(num), ha="center", va="center", fontsize=11,
                fontweight="bold", color=ec)
        ax.text(18, y + 8.0, name, ha="left", va="center", fontsize=8.5,
                fontweight="bold", color=INK)
        ax.text(18, y + 4.6, main, ha="left", va="center", fontsize=7.6, color=INK)
        ax.text(18, y + 1.6, detail, ha="left", va="center", fontsize=6.6, color=GREY)
        y -= 12.6
    # Layers 7-5 are this software; layer 4 down (UDP, IP, Ethernet) is the OS
    # and the network, not this codebase.
    ax.add_patch(FancyBboxPatch((2, 58.8), 6, 36.7, boxstyle="round,pad=0.4,rounding_size=1",
                                linewidth=1.1, facecolor="#EAF1FA", edgecolor=BLUE))
    ax.text(5, 77, "implemented here", ha="center", va="center",
            fontsize=7.2, color=BLUE, rotation=90)
    ax.add_patch(FancyBboxPatch((2, 8.4), 6, 49.3, boxstyle="round,pad=0.4,rounding_size=1",
                                linewidth=1.1, facecolor="#F1F3F6", edgecolor=GREY))
    ax.text(5, 33, "provided by the OS and network", ha="center", va="center",
            fontsize=7.2, color=GREY, rotation=90)
    _label(ax, 52, 98, "ARINC 615A on the OSI reference model", fs=10, color=BLUE)
    return _save(fig, out, "fig11-osi-model.png")


def protocol_interfaces(out):
    """What actually runs on the wire during an operation, and the integration points."""
    fig, ax = _fig(9, 4.6)

    _label(ax, 50, 97, "Protocols in use during a data-load operation", fs=10, color=BLUE)

    _box(ax, 2, 74, 26, 14, "arinc_615a_operation\nHOST", fc=BLUE_L, ec=BLUE, bold=True, fs=8)
    _box(ax, 72, 74, 26, 14, "Target hardware\nTHA", fc=GREEN_L, ec=GREEN, bold=True, fs=8)

    flows = [
        (66, "ARP  —  resolve target MAC", GREY, ":"),
        (57, "FIND  IRQ / IAN      UDP broadcast : 1001", AMBER, "-"),
        (48, "TFTP RRQ / WRQ + option negotiation     UDP : 59", BLUE, "-"),
        (39, "TFTP DATA / ACK  —  block size default 1468 B", BLUE, "-"),
        (30, "TFTP ERROR  —  carries 615A protocol semantics", RED, "-"),
        (21, "ICMP  —  diagnostics only, not part of the protocol", GREY, ":"),
    ]
    for y, text, colour, ls in flows:
        _arrow(ax, 28, y, 72, y, color=colour, style="<|-|>", lw=1.3, ls=ls)
        _label(ax, 50, y + 3.4, text, fs=7, color=colour)

    ax.plot([28, 28], [18, 72], color=BLUE, lw=1, ls="--")
    ax.plot([72, 72], [18, 72], color=GREEN, lw=1, ls="--")

    _box(ax, 2, 2, 30, 12, "Integration point\ntargets list JSON\n(--targets-list)",
         fc=AMBER_L, ec=AMBER, fs=7)
    _box(ax, 35, 2, 30, 12, "Integration point\nARINC 665 Media Set\nManager directory",
         fc=AMBER_L, ec=AMBER, fs=7)
    _box(ax, 68, 2, 30, 12, "Integration point\nlibrary handler\ncallbacks (C++ API)",
         fc=AMBER_L, ec=AMBER, fs=7)
    return _save(fig, out, "fig12-protocol-interfaces.png")


def deployment(out):
    """How the loader is interfaced to an aircraft / bench network."""
    fig, ax = _fig(9, 3.8)
    _label(ax, 50, 96, "Interfacing the loader to other systems", fs=10, color=BLUE)

    _box(ax, 2, 62, 26, 20, "Maintenance laptop\narinc_615a_operation\n\nstatic IP on the\nload subnet",
         fc=BLUE_L, ec=BLUE, fs=7.5)
    _box(ax, 38, 66, 24, 14, "Ethernet switch\nsingle broadcast\ndomain", fc=GREY_L, ec=GREY, fs=7.5)
    _box(ax, 72, 78, 26, 12, "LRU  A   THW-1", fc=GREEN_L, ec=GREEN, fs=7.5)
    _box(ax, 72, 62, 26, 12, "LRU  B   THW-2", fc=GREEN_L, ec=GREEN, fs=7.5)
    _box(ax, 72, 46, 26, 12, "LRU  C   THW-3", fc=GREEN_L, ec=GREEN, fs=7.5)

    _arrow(ax, 28, 72, 38, 72, color=BLUE, style="<|-|>")
    for y in (84, 68, 52):
        _arrow(ax, 62, 72, 72, y, color=GREEN, style="<|-|>")

    _box(ax, 2, 30, 96, 22,
         "Requirements for the network path\n\n"
         "•  Host and targets must share one broadcast domain, or FIND cannot discover them.\n"
         "•  UDP 59 (data load) and UDP 1001 (FIND) must not be filtered between host and target.\n"
         "•  Routers block broadcast: use --target-address=<ip> to address a target directly.\n"
         "•  On a multi-homed host set --local-tftp-address and --local-find-address explicitly.",
         fc=AMBER_L, ec=AMBER, fs=7.5)
    _box(ax, 2, 6, 96, 20,
         "Embedding the loader in another system\n\n"
         "•  As a process:  drive the CLI and read its exit code;  --targets-list gives machine-readable discovery.\n"
         "•  As a library:  link lib/arinc_615a and implement the ...OperationHandler interfaces to receive\n"
         "   progress, status and completion callbacks;  the CLI itself is only a thin consumer of that API.\n"
         "•  Loads are supplied as ARINC 665 media sets;  ARINC 649 supplies the shared check-value functions.",
         fc=GREY_L, ec=GREY, fs=7.5)
    return _save(fig, out, "fig13-deployment.png")


ALL = [system_context, layer_stack, install_windows, install_linux,
       find_sequence, information_sequence, upload_sequence, abort_model,
       test_flow, install_layout, osi_model, protocol_interfaces, deployment]


def render_all(outdir):
    out = pathlib.Path(outdir)
    out.mkdir(parents=True, exist_ok=True)
    return {fn.__name__: fn(out) for fn in ALL}


if __name__ == "__main__":
    import sys
    target = sys.argv[1] if len(sys.argv) > 1 else "."
    for name, path in render_all(target).items():
        print(name, "->", path)
