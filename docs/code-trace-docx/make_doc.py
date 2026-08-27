"""Build the Word report: splice in the extra block diagrams, rasterise every
figure, then hand the result to build_docx.

The pristine artifact export is never modified. Each run regenerates
`arinc615a-cli-engineering.figured.html` from it, so the step is idempotent.

Pass --skip-figures to rebuild the document from the PNGs already in figs/,
which skips the slow Chrome pass.

Figures are rasterised with headless Chrome rather than svglib: Chrome resolves
`currentColor`, the page-level `.sv-*` typography classes and real `marker-end`
arrowheads, none of which svglib handles without preprocessing.
"""
import os
import subprocess
import sys

from bs4 import BeautifulSoup

import block_diagrams

HERE = os.path.dirname(os.path.abspath(__file__))
PRISTINE = os.path.join(HERE, "arinc615a-cli-engineering.html")
FIGURED = os.path.join(HERE, "arinc615a-cli-engineering.figured.html")
FIGS = os.path.join(HERE, "figs")
# the finished document sits with the other docs, one level up
OUT = os.path.join(os.path.dirname(HERE), "ARINC-615A-CLI-Engineering-Trace.docx")
CHROME = r"C:\Program Files\Google\Chrome\Application\chrome.exe"

# Typography for the standalone render page. The published artifact pulls IBM Plex
# from Google Fonts; the renderer runs offline, so each family falls back locally.
RENDER_CSS = """
html,body{margin:0;padding:0;background:#FFFFFF;color:#161A20}
#wrap{display:inline-block;padding:14px;background:#FFFFFF}
svg{display:block;color:#161A20}
.sv-l{font-family:"IBM Plex Sans Condensed","Segoe UI Semibold","Segoe UI",sans-serif;
      font-size:12px;font-weight:600}
.sv-m{font-family:"IBM Plex Mono","Cascadia Mono",Consolas,monospace;font-size:11px}
.sv-s{font-family:"IBM Plex Mono","Cascadia Mono",Consolas,monospace;font-size:10.5px}
"""


def splice(soup):
    """Insert each extra figure at its anchor. Returns the number inserted."""
    added = 0
    for section_id, anchor, builder, caption in block_diagrams.FIGURES:
        section = soup.find("section", id=section_id)
        if section is None:
            raise SystemExit(f"section {section_id} not found")

        figure = BeautifulSoup(
            f'<figure class="added"><div class="fw">{builder()}</div>'
            f"<figcaption>{caption}</figcaption></figure>",
            "html.parser",
        ).figure

        if anchor == "end":
            section.append(figure)
        else:
            tag, _, cls = anchor.partition(".")
            target = section.find(tag, class_=cls or None, recursive=False)
            if target is None:
                raise SystemExit(f"anchor {anchor} not found in {section_id}")
            target.insert_after(figure)
        added += 1
    return added


def viewbox(svg):
    """SVG attribute names are lower-cased by the HTML parser."""
    return svg.get("viewBox") or svg.get("viewbox") or "0 0 700 400"


def rasterise(soup):
    os.makedirs(FIGS, exist_ok=True)
    svgs = soup.find_all("svg")
    written = []
    for index, svg in enumerate(svgs, start=1):
        _, _, w, h = viewbox(svg).split()
        w, h = int(float(w)), int(float(h))
        svg["width"], svg["height"] = str(w), str(h)

        page_path = os.path.join(FIGS, f"_fig{index}.html")
        png_path = os.path.join(FIGS, f"fig{index}.png")
        with open(page_path, "w", encoding="utf-8") as handle:
            handle.write('<!doctype html><meta charset="utf-8">'
                         f"<style>{RENDER_CSS}</style><div id=wrap>{svg}</div>")

        subprocess.run(
            [CHROME, "--headless=new", "--disable-gpu", "--no-sandbox",
             "--disable-extensions", "--disable-background-networking",
             "--hide-scrollbars", "--force-device-scale-factor=3",
             "--default-background-color=FFFFFFFF",
             f"--window-size={w + 28},{h + 28}", "--virtual-time-budget=1200",
             f"--screenshot={png_path}", "file:///" + page_path.replace("\\", "/")],
            capture_output=True, timeout=120,
        )
        os.remove(page_path)
        size = os.path.getsize(png_path) if os.path.exists(png_path) else 0
        if size < 2000:
            raise SystemExit(f"fig{index} did not render ({size} bytes)")
        written.append((index, w, h, size))
        print(f"  fig{index:<3} {w}x{h}  {size // 1024} KB")

    # drop any PNG left over from a previous run with more figures
    stale = len(svgs) + 1
    while os.path.exists(os.path.join(FIGS, f"fig{stale}.png")):
        os.remove(os.path.join(FIGS, f"fig{stale}.png"))
        stale += 1
    return written


def main():
    os.chdir(HERE)
    soup = BeautifulSoup(open(PRISTINE, encoding="utf-8").read(), "html.parser")

    original = len(soup.find_all("figure"))
    added = splice(soup)
    print(f"figures: {original} from the artifact + {added} added = {original + added}")

    with open(FIGURED, "w", encoding="utf-8") as handle:
        handle.write(str(soup))

    if "--skip-figures" in sys.argv:
        missing = [i for i in range(1, original + added + 1)
                   if not os.path.exists(os.path.join(FIGS, f"fig{i}.png"))]
        if missing:
            raise SystemExit(f"--skip-figures but fig{missing[0]}.png is absent")
        print("rasterising: skipped, reusing figs/")
    else:
        print("rasterising:")
        rasterise(BeautifulSoup(str(soup), "html.parser"))

    import build_docx
    build_docx.SRC = os.path.basename(FIGURED)
    build_docx.OUT = OUT
    build_docx.FIG_PATHS.clear()
    build_docx.main()


if __name__ == "__main__":
    sys.exit(main())
