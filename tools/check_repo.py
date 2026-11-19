#!/usr/bin/env python3
"""Repository checks run in CI (no Unreal Engine needed).

- Every .uproject is valid JSON and targets the same engine version.
- No generated folders, build outputs or office documents are committed.
- Every relative link and image in the Markdown files points to an existing file.
"""
import json
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
EXPECTED_ENGINE = "5.8"
FORBIDDEN_DIRS = {"Binaries", "Intermediate", "Saved", "DerivedDataCache", ".vs"}
FORBIDDEN_EXT = {".pdf", ".docx", ".doc", ".exe", ".dll", ".pdb", ".lnk", ".url", ".zip", ".7z"}

errors: list[str] = []


def tracked_files() -> list[pathlib.Path]:
    out = subprocess.run(["git", "ls-files", "-z"], cwd=ROOT, capture_output=True, check=True).stdout
    return [ROOT / p for p in out.decode("utf-8").split("\0") if p]


files = tracked_files()

# 1. Unreal project descriptors
uprojects = [f for f in files if f.suffix == ".uproject"]
if not uprojects:
    errors.append("no .uproject files found")
for f in uprojects:
    try:
        data = json.loads(f.read_text(encoding="utf-8-sig"))
    except json.JSONDecodeError as e:
        errors.append(f"{f.relative_to(ROOT)}: invalid JSON ({e})")
        continue
    if data.get("EngineAssociation") != EXPECTED_ENGINE:
        errors.append(f"{f.relative_to(ROOT)}: EngineAssociation is {data.get('EngineAssociation')!r}, expected {EXPECTED_ENGINE!r}")

# 2. Files that must never be committed
for f in files:
    rel = f.relative_to(ROOT)
    if FORBIDDEN_DIRS.intersection(rel.parts):
        errors.append(f"{rel}: generated folder committed")
    if f.suffix.lower() in FORBIDDEN_EXT:
        errors.append(f"{rel}: '{f.suffix}' files must not be committed")

# 3. Relative links in Markdown
link_re = re.compile(r"!?\[[^\]]*\]\(([^)\s]+)(?:\s+\"[^\"]*\")?\)")
for md in (f for f in files if f.suffix == ".md"):
    text = re.sub(r"```.*?```", "", md.read_text(encoding="utf-8"), flags=re.S)
    for target in link_re.findall(text):
        if re.match(r"^[a-z]+:", target) or target.startswith("#"):
            continue
        path = (md.parent / target.split("#")[0]).resolve()
        if not path.exists():
            errors.append(f"{md.relative_to(ROOT)}: broken link -> {target}")

if errors:
    print("Repository check FAILED:")
    for e in errors:
        print("  -", e)
    sys.exit(1)
print(f"Repository check passed: {len(uprojects)} projects on UE {EXPECTED_ENGINE}, {len(files)} tracked files, Markdown links OK.")
