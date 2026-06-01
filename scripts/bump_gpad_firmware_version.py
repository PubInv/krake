#!/usr/bin/env python3
"""Increment the GPAD firmware patch version when a pull request is raised."""

from __future__ import annotations

import argparse
import re
from pathlib import Path

VERSION_PATH = Path("Firmware/GPAD_API/FIRMWARE_VERSION")
SEMVER_RE = re.compile(
    r"^(?P<major>0|[1-9]\d*)\."
    r"(?P<minor>0|[1-9]\d*)\."
    r"(?P<patch>0|[1-9]\d*)"
    r"(?:-(?P<prerelease>[0-9A-Za-z-]+(?:\.[0-9A-Za-z-]+)*))?"
    r"(?:\+(?P<metadata>[0-9A-Za-z-]+(?:\.[0-9A-Za-z-]+)*))?$"
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pr-number", type=int, required=True, help="GitHub pull request number")
    parser.add_argument("--version-file", type=Path, default=VERSION_PATH)
    return parser.parse_args()


def bump_version(version: str, pr_number: int) -> str:
    if pr_number < 1:
        raise ValueError("pull request number must be positive")
    match = SEMVER_RE.fullmatch(version.strip())
    if match is None:
        raise ValueError(f"invalid semantic version: {version!r}")
    metadata = match.group("metadata") or ""
    if metadata == f"pr.{pr_number}":
        return version.strip()
    return f"{match.group('major')}.{match.group('minor')}.{int(match.group('patch')) + 1}+pr.{pr_number}"


def main() -> None:
    args = parse_args()
    current = args.version_file.read_text(encoding="utf-8").strip()
    updated = bump_version(current, args.pr_number)
    if updated != current:
        args.version_file.write_text(updated + "\n", encoding="utf-8")
        print(f"Bumped GPAD firmware version: {current} -> {updated}")
    else:
        print(f"GPAD firmware version already records PR #{args.pr_number}: {current}")


if __name__ == "__main__":
    main()
