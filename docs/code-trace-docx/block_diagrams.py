"""Block diagrams added to the engineering trace on top of the artifact's own figures.

Each entry is (section id, anchor rule, svg markup, caption html). The anchor rule
says where in the section the figure is spliced: "end" appends it, otherwise it is
inserted immediately after the first child matching the given CSS-ish selector.

Facts here are taken from the source tree, not from the prose:
  - the initialisation file is a host-side TFTP *read* for every operation
    (OperationImpl::initialise -> tftpClientV->readOperation(), OperationImpl.cpp:262)
  - request/answer files are host-side TFTP writes (UploadOperationImpl.cpp:106 and peers)
  - extension-to-operation mapping is lib/arinc_615a/files/Files.hpp:32-43
  - link dependencies are the target_link_libraries() calls in lib/ and app/
"""

INK = "currentColor"
ACCENT = "#A85D00"
TEAL = "#0C6B69"
DANGER = "#A6301F"

# Arrow markers are declared per-figure so each SVG stays self-contained.
MARKERS = """
  <defs>
    <marker id="{p}k" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7"
            orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" fill="currentColor"/></marker>
    <marker id="{p}a" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7"
            orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" fill="#A85D00"/></marker>
    <marker id="{p}t" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7"
            orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" fill="#0C6B69"/></marker>
  </defs>
"""


def box(x, y, w, h, rx=3, op="0.85", extra=""):
    return (f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{rx}" fill="none" '
            f'stroke="currentColor" opacity="{op}" {extra}/>')


# --------------------------------------------------------------- 1  port map

def _port_map():
    p = []
    p.append(MARKERS.format(p="pm"))
    # panels
    p.append(box(18, 42, 232, 190, rx=4))
    p.append(box(450, 42, 232, 190, rx=4))
    p.append(f'<text class="sv-l" x="134" y="32" text-anchor="middle" fill="{ACCENT}">HOST — arinc_615a_operation</text>')
    p.append(f'<text class="sv-l" x="566" y="32" text-anchor="middle" fill="{TEAL}">TARGET HARDWARE</text>')

    host = [("FIND client", "sends from :1001 or dynamic"),
            ("TFTP client", "initiates the first transfer"),
            ("TFTP server", "binds :59, or dynamic port")]
    tgt = [("FIND server", "listens on UDP :1001"),
           ("TFTP server", "listens on UDP :59"),
           ("TFTP client", "pushes status + data files")]
    for i, ((ht, hs), (tt, ts)) in enumerate(zip(host, tgt)):
        y = 56 + i * 56
        p.append(box(32, y, 204, 44, op="0.55"))
        p.append(box(464, y, 204, 44, op="0.55"))
        p.append(f'<text class="sv-m" x="44" y="{y + 18}" fill="currentColor">{ht}</text>')
        p.append(f'<text class="sv-s" x="44" y="{y + 34}" fill="currentColor" opacity="0.62">{hs}</text>')
        p.append(f'<text class="sv-m" x="476" y="{y + 18}" fill="currentColor">{tt}</text>')
        p.append(f'<text class="sv-s" x="476" y="{y + 34}" fill="currentColor" opacity="0.62">{ts}</text>')

    p.append('<text class="sv-s" x="350" y="50" text-anchor="middle" fill="currentColor" opacity="0.5">Ethernet · UDP</text>')

    def arrow(y, right, colour, mk, label):
        x1, x2 = (240, 460) if right else (460, 240)
        p.append(f'<line x1="{x1}" y1="{y}" x2="{x2}" y2="{y}" stroke="{colour}" '
                 f'stroke-width="1.2" marker-end="url(#pm{mk})"/>')
        p.append(f'<text class="sv-s" x="350" y="{y - 6}" text-anchor="middle" fill="{colour}">{label}</text>')

    arrow(74, True, ACCENT, "a", "FIND request")
    arrow(94, False, TEAL, "t", "FIND answer")
    arrow(138, True, ACCENT, "a", "TFTP RRQ / WRQ → :59")
    arrow(194, False, TEAL, "t", "TFTP WRQ → host port")

    p.append('<text class="sv-s" x="18" y="258" fill="currentColor" opacity="0.66">The host initiates only the first transfer of each operation; everything after that is target-initiated.</text>')
    p.append('<text class="sv-s" x="18" y="274" fill="currentColor" opacity="0.66">Ports: FIND 1001 · TFTP well-known 59 · the Port Option swaps the host&#8217;s 59 for an OS-assigned port.</text>')
    return ('<svg viewBox="0 0 700 288" role="img" aria-label="Block diagram of the host data '
            'loader and target hardware showing the UDP ports each side binds and which side '
            'initiates each transfer">' + "".join(p) + "</svg>")


# --------------------------------------------------------------- 2  dispatch

def _dispatch():
    p = [MARKERS.format(p="dp")]
    stages = [
        ("main()", "log levels · registry · io_context · jthread"),
        ("Utils_commandLineHandler( registry )", "matches argv[1] against a registered name"),
        ("CommandRegistry::instance()", "name → Command object (shared_ptr)"),
        ("Command::execute( args )", "program_options parse · resolve target address"),
        ("Host::create…Operation( ioContext, config )", "handler interface is implemented by the command"),
        ("operation-&gt;start()   ·   done.wait()", "protocol runs on the I/O thread until finished()"),
    ]
    for i, (title, sub) in enumerate(stages):
        y = 34 + i * 52
        p.append(box(40, y, 334, 40))
        p.append(f'<line x1="41" y1="{y + 1}" x2="41" y2="{y + 39}" stroke="{ACCENT}" stroke-width="2.5"/>')
        p.append(f'<text class="sv-m" x="54" y="{y + 17}" fill="currentColor">{title}</text>')
        p.append(f'<text class="sv-s" x="54" y="{y + 32}" fill="currentColor" opacity="0.62">{sub}</text>')
        if i < len(stages) - 1:
            p.append(f'<line x1="207" y1="{y + 40}" x2="207" y2="{y + 50}" stroke="currentColor" '
                     f'stroke-width="1.2" opacity="0.7" marker-end="url(#dpk)"/>')

    # registry side panel, hung off stage 3
    p.append(box(400, 124, 282, 110, rx=4, op="0.45", extra='stroke-dasharray="4 3"'))
    p.append(f'<text class="sv-s" x="412" y="142" fill="{ACCENT}">REGISTERED NAMES</text>')
    for j, line in enumerate(["Find · Targets · Information",
                              "Upload · AdhocUpload · UploadLoads",
                              "BatchUpload · MedDownload · OpDownload",
                              "+ ARINC 665 media-set commands"]):
        op = "0.5" if j == 3 else "0.8"
        p.append(f'<text class="sv-s" x="412" y="{162 + j * 18}" fill="currentColor" opacity="{op}">{line}</text>')
    p.append('<line x1="374" y1="158" x2="400" y2="158" stroke="currentColor" stroke-width="1" '
             'opacity="0.4" stroke-dasharray="3 3"/>')

    p.append('<text class="sv-s" x="40" y="352" fill="currentColor" opacity="0.6">Nothing on this path is ARINC 615A-specific until stage 5.</text>')
    return ('<svg viewBox="0 0 700 364" role="img" aria-label="Pipeline from the process argument '
            'vector through the command registry to a running ARINC 615A operation">'
            + "".join(p) + "</svg>")


# --------------------------------------------------------------- sequences

def _sequence(title_l, title_r, msgs, phases, footer, height, aria,
              host_bar, tgt_bar, highlight=None):
    """Shared two-lifeline sequence renderer.

    msgs: (y, direction, colour-key, label). direction 'r' host->target, 'l' target->host,
    'self' draws a short self-call on the host lifeline.
    """
    p = [MARKERS.format(p="sq")]
    p.append(box(60, 8, 180, 26))
    p.append(box(430, 8, 180, 26))
    p.append(f'<text class="sv-l" x="150" y="26" text-anchor="middle" fill="{ACCENT}">{title_l}</text>')
    p.append(f'<text class="sv-l" x="520" y="26" text-anchor="middle" fill="{TEAL}">{title_r}</text>')
    bottom = height - 30
    for x in (150, 520):
        p.append(f'<line x1="{x}" y1="34" x2="{x}" y2="{bottom}" stroke="currentColor" '
                 f'stroke-width="1" stroke-dasharray="3 4" opacity="0.45"/>')
    p.append(f'<rect x="144" y="{host_bar[0]}" width="12" height="{host_bar[1]}" fill="currentColor" opacity="0.12"/>')
    p.append(f'<rect x="514" y="{tgt_bar[0]}" width="12" height="{tgt_bar[1]}" fill="currentColor" opacity="0.12"/>')

    if highlight:
        hy, hh, hlabel = highlight
        p.append(f'<rect x="132" y="{hy}" width="404" height="{hh}" rx="3" fill="{ACCENT}" opacity="0.08"/>')
        p.append(f'<text class="sv-s" x="546" y="{hy + 18}" fill="{ACCENT}">{hlabel}</text>')

    for y, direction, key, label in msgs:
        colour = {"a": ACCENT, "t": TEAL, "k": INK}[key]
        if direction == "self":
            p.append(f'<line x1="158" y1="{y}" x2="196" y2="{y}" stroke="{colour}" stroke-width="1.2"/>')
            p.append(f'<line x1="196" y1="{y}" x2="196" y2="{y + 10}" stroke="{colour}" stroke-width="1.2"/>')
            p.append(f'<line x1="196" y1="{y + 10}" x2="160" y2="{y + 10}" stroke="{colour}" '
                     f'stroke-width="1.2" marker-end="url(#sq{key})"/>')
            p.append(f'<text class="sv-s" x="206" y="{y + 8}" fill="{colour}">{label}</text>')
            continue
        x1, x2 = (158, 512) if direction == "r" else (512, 158)
        p.append(f'<line x1="{x1}" y1="{y}" x2="{x2}" y2="{y}" stroke="{colour}" '
                 f'stroke-width="1.2" marker-end="url(#sq{key})"/>')
        p.append(f'<text class="sv-s" x="335" y="{y - 6}" text-anchor="middle" fill="{colour}">{label}</text>')

    for y, text in phases:
        p.append(f'<text class="sv-s" x="14" y="{y}" fill="{ACCENT}" opacity="0.85">{text}</text>')

    p.append(f'<text class="sv-s" x="14" y="{height - 10}" fill="currentColor" opacity="0.62">{footer}</text>')
    return f'<svg viewBox="0 0 700 {height}" role="img" aria-label="{aria}">' + "".join(p) + "</svg>"


def _upload_seq():
    return _sequence(
        "Host data loader", "Target hardware",
        [(64, "r", "a", "TFTP RRQ   &lt;TID&gt;.LUI     initialisation"),
         (96, "l", "t", "TFTP WRQ   &lt;TID&gt;.LUS     OperationAccepted"),
         (132, "r", "a", "TFTP WRQ   &lt;TID&gt;.LUR     load list"),
         (176, "l", "t", "TFTP RRQ   load files   × N   — served from the media set"),
         (210, "l", "t", "TFTP WRQ   &lt;TID&gt;.LUS     progress   × N"),
         (252, "l", "t", "TFTP WRQ   &lt;TID&gt;.LUS     final status"),
         (276, "self", "k", "finished() → done.count_down()")],
        [(70, "INIT"), (138, "LIST"), (196, "XFER"), (258, "END")],
        "The host runs a TFTP server for the whole operation, answering the target&#8217;s read request for each load file.",
        320,
        "Message sequence for the ARINC 615A upload operation between host data loader and target hardware",
        host_bar=(50, 240), tgt_bar=(72, 190),
        highlight=(120, 26, "sent from status()"))


def _mdd_seq():
    return _sequence(
        "Host data loader", "Target hardware",
        [(64, "r", "a", "TFTP RRQ   &lt;TID&gt;.LND     initialisation"),
         (96, "l", "t", "TFTP WRQ   &lt;TID&gt;.LNS     OperationAccepted"),
         (132, "r", "a", "TFTP WRQ   &lt;TID&gt;.LNR     filenames + user-defined data"),
         (176, "l", "t", "TFTP WRQ   &lt;file&gt;   × N   — target-defined names"),
         (210, "l", "t", "TFTP WRQ   &lt;TID&gt;.LNS     progress   × N"),
         (250, "l", "t", "TFTP WRQ   &lt;TID&gt;.LNS     final status")],
        [(70, "INIT"), (138, "REQ"), (196, "XFER"), (256, "END")],
        "Every data-file name comes from the target; the host passes it through Helper::normaliseFilename() first.",
        296,
        "Message sequence for the ARINC 615A media defined download operation",
        host_bar=(50, 216), tgt_bar=(72, 190),
        highlight=(120, 26, "sent from status()"))


def _odd_seq():
    return _sequence(
        "Host data loader", "Target hardware",
        [(64, "r", "a", "TFTP RRQ   &lt;TID&gt;.LNO     initialisation"),
         (96, "l", "t", "TFTP WRQ   &lt;TID&gt;.LNS     OperationAccepted"),
         (134, "l", "t", "TFTP WRQ   &lt;TID&gt;.LNL     list of available files"),
         (168, "r", "a", "TFTP WRQ   &lt;TID&gt;.LNA     operator&#8217;s selection"),
         (208, "l", "t", "TFTP WRQ   &lt;file&gt;   × N"),
         (240, "l", "t", "TFTP WRQ   &lt;TID&gt;.LNS     progress   × N"),
         (274, "l", "t", "TFTP WRQ   &lt;TID&gt;.LNS     final status")],
        [(70, "INIT"), (140, "ADVERTISE"), (214, "XFER"), (280, "END")],
        "The .LNL is not integrity-checked &#8212; unlike the Information list handler, no checksum option is accepted.",
        320,
        "Message sequence for the ARINC 615A operator defined download operation, showing the "
        "extra list-and-answer round trip",
        host_bar=(50, 240), tgt_bar=(72, 214),
        highlight=(120, 62, "extra round trip"))


# --------------------------------------------------------------- 6  file map

def _file_map():
    p = []
    cols = [("Information", "&lt;TID&gt;.LC*"), ("Upload", "&lt;TID&gt;.LU*"),
            ("Media Defined", "Download"), ("Operator Defined", "Download")]
    rows = [("Initialisation", "host reads — RRQ", "a", [".LCI", ".LUI", ".LND", ".LNO"]),
            ("Request / Answer", "host writes — WRQ", "a", [".LUR", ".LNR", ".LNA"]),
            ("List", "target writes", "t", [".LCL", ".LNL"]),
            ("Status", "target writes", "t", [".LCS", ".LUS", ".LNS", ".LNS"])]
    cell = {0: {0: ".LCI", 1: ".LUI", 2: ".LND", 3: ".LNO"},
            1: {1: ".LUR", 2: ".LNR", 3: ".LNA"},
            2: {0: ".LCL", 3: ".LNL"},
            3: {0: ".LCS", 1: ".LUS", 2: ".LNS", 3: ".LNS"}}

    cx = [142, 278, 414, 550]
    cw = 132
    for i, (c1, c2) in enumerate(cols):
        p.append(f'<text class="sv-l" x="{cx[i] + cw // 2}" y="{26}" text-anchor="middle" fill="currentColor">{c1}</text>')
        p.append(f'<text class="sv-s" x="{cx[i] + cw // 2}" y="{40}" text-anchor="middle" fill="currentColor" opacity="0.55">{c2}</text>')

    for r, (label, sub, key, _) in enumerate(rows):
        y = 52 + r * 46
        colour = ACCENT if key == "a" else TEAL
        p.append(f'<text class="sv-m" x="132" y="{y + 19}" text-anchor="end" fill="currentColor">{label}</text>')
        p.append(f'<text class="sv-s" x="132" y="{y + 33}" text-anchor="end" fill="{colour}" opacity="0.9">{sub}</text>')
        for c in range(4):
            value = cell[r].get(c)
            if value:
                p.append(f'<rect x="{cx[c]}" y="{y}" width="{cw}" height="40" rx="3" '
                         f'fill="{colour}" opacity="0.10"/>')
                p.append(box(cx[c], y, cw, 40, op="0.5"))
                p.append(f'<text class="sv-m" x="{cx[c] + cw // 2}" y="{y + 25}" '
                         f'text-anchor="middle" fill="currentColor">{value}</text>')
            else:
                p.append(box(cx[c], y, cw, 40, op="0.18", extra='stroke-dasharray="3 3"'))
                p.append(f'<text class="sv-s" x="{cx[c] + cw // 2}" y="{y + 25}" '
                         f'text-anchor="middle" fill="currentColor" opacity="0.28">—</text>')

    p.append('<text class="sv-s" x="18" y="256" fill="currentColor" opacity="0.62">All four name files as &lt;THW ID&gt;_&lt;Position&gt;.&lt;ext&gt;; both download variants share one status extension.</text>')
    p.append('<text class="sv-s" x="18" y="272" fill="currentColor" opacity="0.62">Mapping: lib/arinc_615a/files/Files.hpp:32–43. Data files carry target-chosen names and are not listed.</text>')
    return ('<svg viewBox="0 0 700 284" role="img" aria-label="Matrix of ARINC 615A protocol file '
            'extensions by operation and by transfer direction">' + "".join(p) + "</svg>")


# --------------------------------------------------------------- 7  timers

def _timers():
    p = [MARKERS.format(p="tm")]
    p.append(f'<rect x="30" y="40" width="430" height="210" rx="4" fill="{ACCENT}" opacity="0.05"/>')
    p.append(f'<rect x="30" y="40" width="430" height="210" rx="4" fill="none" stroke="{ACCENT}" stroke-width="1.4"/>')
    p.append(f'<text class="sv-l" x="44" y="62" fill="{ACCENT}">DLP timeout · 13 s</text>')
    p.append('<text class="sv-s" x="44" y="78" fill="currentColor" opacity="0.68">watchdog on the whole operation,</text>')
    p.append('<text class="sv-s" x="44" y="93" fill="currentColor" opacity="0.68">re-armed by every status file — --dlp-timeout</text>')

    p.append(box(54, 108, 382, 118, rx=4, op="0.7"))
    p.append('<text class="sv-l" x="68" y="130" fill="currentColor">DLP retries · 1</text>')
    p.append('<text class="sv-s" x="68" y="146" fill="currentColor" opacity="0.68">one whole TFTP transfer, re-issued — --dlp-retries</text>')

    p.append(box(78, 160, 334, 56, rx=4, op="0.7"))
    p.append('<text class="sv-l" x="92" y="182" fill="currentColor">TFTP packet timeout · 2 s, 1 retry</text>')
    p.append('<text class="sv-s" x="92" y="198" fill="currentColor" opacity="0.68">a single packet — --tftp-timeout</text>')

    p.append(box(490, 40, 192, 74, rx=4, op="0.4", extra='stroke-dasharray="4 3"'))
    p.append('<text class="sv-l" x="504" y="62" fill="currentColor" opacity="0.8">FIND window · 3 s</text>')
    p.append('<text class="sv-s" x="504" y="78" fill="currentColor" opacity="0.6">a separate scope: fixed</text>')
    p.append('<text class="sv-s" x="504" y="92" fill="currentColor" opacity="0.6">listening period — --timeout</text>')

    p.append(f'<text class="sv-s" x="490" y="146" fill="{ACCENT}">Only the DLP timeout ends</text>')
    p.append(f'<text class="sv-s" x="490" y="162" fill="{ACCENT}">an operation unconditionally.</text>')
    p.append('<text class="sv-s" x="490" y="188" fill="currentColor" opacity="0.6">Effective value is</text>')
    p.append('<text class="sv-s" x="490" y="203" fill="currentColor" opacity="0.6">max( exceptionTimer,</text>')
    p.append('<text class="sv-s" x="490" y="218" fill="currentColor" opacity="0.6">dlpTimeout ).</text>')

    # graceful abort only makes progress when the target next asks to write a status file
    chain = [(30, 168, "1st Ctrl-C → abort", "recorded, not sent"),
             (222, 218, "next status request", "isAborted() sends ABORT"),
             (474, 208, "target closes out", "with its own final status")]
    for x, w, t1, t2 in chain:
        p.append(box(x, 272, w, 40, op="0.6"))
        p.append(f'<text class="sv-m" x="{x + 12}" y="290" fill="currentColor">{t1}</text>')
        p.append(f'<text class="sv-s" x="{x + 12}" y="305" fill="currentColor" opacity="0.62">{t2}</text>')
    for x1, x2 in ((198, 218), (440, 470)):
        p.append(f'<line x1="{x1}" y1="292" x2="{x2}" y2="292" stroke="currentColor" '
                 f'stroke-width="1.2" opacity="0.7" marker-end="url(#tmk)"/>')

    p.append(f'<text class="sv-s" x="30" y="334" fill="{DANGER}">2nd Ctrl-C → terminate: finished() is called directly with a locally synthesised status.</text>')
    return ('<svg viewBox="0 0 700 346" role="img" aria-label="Nested timer scopes with the DLP '
            'timeout enclosing the DLP retry which encloses the TFTP packet timeout, plus the '
            'abort escalation chain">' + "".join(p) + "</svg>")


# --------------------------------------------------------------- 8  build graph

def _build_graph():
    p = [MARKERS.format(p="bg")]
    p.append(f'<text class="sv-s" x="18" y="30" fill="{ACCENT}">APPLICATIONS</text>')
    p.append(f'<text class="sv-s" x="348" y="30" fill="{ACCENT}">PROJECT LIBRARIES</text>')
    p.append(f'<text class="sv-s" x="558" y="30" fill="{ACCENT}">DEPENDENCIES</text>')

    apps = [("arinc_615a_operation", True), ("arinc_615a_download_request_file", False),
            ("arinc_615a_test_tha", False), ("arinc_615a_unit_test", False)]
    for i, (name, built) in enumerate(apps):
        y = 50 + i * 46
        if built:
            p.append(f'<rect x="18" y="{y}" width="300" height="38" rx="3" fill="{ACCENT}" opacity="0.08"/>')
            p.append(f'<rect x="18" y="{y}" width="300" height="38" rx="3" fill="none" stroke="{ACCENT}" stroke-width="1.4"/>')
            p.append(f'<text class="sv-m" x="32" y="{y + 24}" fill="currentColor">{name}</text>')
        else:
            p.append(box(18, y, 300, 38, op="0.35", extra='stroke-dasharray="4 3"'))
            p.append(f'<text class="sv-m" x="32" y="{y + 24}" fill="currentColor" opacity="0.55">{name}</text>')

    libs = [("arinc_615a_commands", 50, True), ("arinc_615a", 110, True), ("arinc_615a_test", 170, False)]
    for name, y, built in libs:
        op, sw = (ACCENT, "1.4") if built else ("currentColor", "1")
        dash = "" if built else 'stroke-dasharray="4 3"'
        if built:
            p.append(f'<rect x="348" y="{y}" width="180" height="38" rx="3" fill="{ACCENT}" opacity="0.08"/>')
        p.append(f'<rect x="348" y="{y}" width="180" height="38" rx="3" fill="none" stroke="{op}" '
                 f'stroke-width="{sw}" {dash} opacity="{"1" if built else "0.35"}"/>')
        p.append(f'<text class="sv-m" x="360" y="{y + 24}" fill="currentColor" '
                 f'opacity="{"1" if built else "0.55"}">{name}</text>')

    # ordered so the two fan-outs (from arinc_615a_commands and arinc_615a) do not cross
    exts = [("commands", 44), ("arinc_665", 84), ("tftp", 124), ("arinc_649", 164),
            ("helper", 204), ("Boost · spdlog", 244)]
    for name, y in exts:
        p.append(box(558, y, 124, 32, op="0.5"))
        p.append(f'<text class="sv-s" x="570" y="{y + 20}" fill="currentColor">{name}</text>')

    def edge(x1, y1, x2, y2, colour=INK, mk="k", op="0.55"):
        p.append(f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="{colour}" '
                 f'stroke-width="1.1" opacity="{op}" marker-end="url(#bg{mk})"/>')

    edge(318, 69, 344, 69, ACCENT, "a", "0.9")     # operation -> commands lib
    edge(318, 115, 344, 129, op="0.35")            # drf -> arinc_615a
    edge(318, 161, 344, 133, op="0.35")            # test_tha -> arinc_615a
    edge(318, 207, 344, 193, op="0.35")            # unit_test -> arinc_615a_test
    edge(438, 88, 438, 106, ACCENT, "a", "0.9")    # commands lib -> arinc_615a
    edge(528, 69, 554, 60, ACCENT, "a", "0.6")     # commands lib -> commands
    edge(528, 69, 554, 100, ACCENT, "a", "0.6")    # commands lib -> arinc_665
    for _, ey in exts[2:]:
        edge(528, 129, 554, ey + 16)               # arinc_615a -> its PUBLIC deps

    p.append(f'<rect x="18" y="290" width="14" height="10" rx="2" fill="{ACCENT}" opacity="0.25"/>')
    p.append(f'<text class="sv-s" x="38" y="299" fill="currentColor" opacity="0.68">built by this CLI-only working copy</text>')
    p.append('<rect x="300" y="290" width="14" height="10" rx="2" fill="none" stroke="currentColor" opacity="0.35" stroke-dasharray="3 2"/>')
    p.append('<text class="sv-s" x="320" y="299" fill="currentColor" opacity="0.68">present on disk, not wired into the build</text>')
    return ('<svg viewBox="0 0 700 312" role="img" aria-label="Dependency graph from the CLI '
            'applications through the project libraries to their external dependencies, marking '
            'which targets this working copy actually builds">' + "".join(p) + "</svg>")


# --------------------------------------------------------------- registry

FIGURES = [
    ("s1", "end", _port_map,
     "Two processes, four sockets. The host owns the FIND client and a TFTP client/server pair; "
     "the target mirrors it. Which side <em>listens</em> is fixed by the standard, which is why the "
     "host&#8217;s own TFTP server has to be running before the first request goes out."),
    ("s5", "div.prose", _dispatch,
     "The dispatch path. Stages 1&#8211;4 are generic <code>Commands</code> machinery shared with the "
     "ARINC 665 sub-commands; the protocol only appears at stage 5, where the command hands itself "
     "to the operation factory as the handler."),
    ("s11", "div.prose", _upload_seq,
     "Upload. The one host-initiated transfer is the <code>.LUI</code> read; the "
     "<code>.LUR</code> load list follows only after the target has accepted, and every load file "
     "afterwards is <em>pulled</em> by the target from the host&#8217;s TFTP server."),
    ("s12", "div.prose", _mdd_seq,
     "Media Defined Download. Structurally the upload with the data direction reversed: the "
     "<code>.LNR</code> request replaces the <code>.LUR</code> load list, and the target writes the "
     "files instead of reading them."),
    ("s13", "div.prose", _odd_seq,
     "Operator Defined Download. The shaded band is the whole difference from &#167;12 &#8212; one "
     "extra round trip in which the target advertises with <code>.LNL</code> and the host answers "
     "with <code>.LNA</code>."),
    ("s14", "end", _file_map,
     "Which protocol file belongs to which operation, and who writes it. Reading a row across shows "
     "the four operations are the same state machine over different extensions; reading a column "
     "down gives the complete file inventory for one operation."),
    ("s16", "div.tw", _timers,
     "Timer scopes nest strictly: a TFTP packet timeout can fire many times inside one DLP retry, "
     "and DLP retries can be exhausted well inside the DLP watchdog. The bottom row is the graceful "
     "abort path &#8212; note that it only makes progress when the target asks to write a status file."),
    ("s20", "div.prose", _build_graph,
     "Link structure of the tree, with this working copy&#8217;s reduction marked. "
     "<code>arinc_615a_operation</code> also links <code>commands</code> and "
     "<code>arinc_665_commands</code> directly; those edges are omitted here to keep the "
     "library chain readable."),
]
