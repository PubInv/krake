#!/usr/bin/env python3
"""Increment the GPAD firmware version using plain Semantic Versioning."""

from __future__ import annotations

import argparse
import re
import subprocess
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
    parser.add_argument(
        "--bump",
        choices=("auto", "patch", "minor", "major"),
        default="auto",
        help="Version component to increment. auto uses commit messages.",
    )
    parser.add_argument("--version-file", type=Path, default=VERSION_PATH)
    parser.add_argument(
        "--from-ref",
        help="Git ref to compare against when --bump=auto. Defaults to the last version-file commit.",
    )
    return parser.parse_args()


def run_git(args: list[str]) -> str:
    return subprocess.check_output(["git", *args], text=True).strip()


def parse_version(version: str) -> tuple[int, int, int]:
    match = SEMVER_RE.fullmatch(version.strip())
    if match is None:
        raise ValueError(f"invalid semantic version: {version!r}")
    return int(match.group("major")), int(match.group("minor")), int(match.group("patch"))


def commit_messages(version_file: Path, from_ref: str | None) -> str:
    if from_ref:
        return run_git(["log", "--format=%s%n%b", f"{from_ref}..HEAD"])

    version_commit = run_git(["log", "-n", "1", "--format=%H", "--", str(version_file)])
    if not version_commit:
        return run_git(["log", "--format=%s%n%b", "HEAD"])
    return run_git(["log", "--format=%s%n%b", f"{version_commit}..HEAD"])


def select_bump_type(messages: str) -> str:
    if re.search(r"^BREAKING CHANGE:", messages, re.MULTILINE | re.IGNORECASE):
        return "major"
    if re.search(r"^[a-z]+(?:\([^)]+\))?!:", messages, re.MULTILINE | re.IGNORECASE):
        return "major"
    if re.search(r"^(feat|feature)(?:\([^)]+\))?:", messages, re.MULTILINE | re.IGNORECASE):
        return "minor"
    return "patch"


def bump_version(version: str, bump_type: str) -> str:
    major, minor, patch = parse_version(version)
    if bump_type == "major":
        return f"{major + 1}.0.0"
    if bump_type == "minor":
        return f"{major}.{minor + 1}.0"
    if bump_type == "patch":
        return f"{major}.{minor}.{patch + 1}"
    raise ValueError(f"unsupported bump type: {bump_type!r}")


def main() -> None:
    args = parse_args()
    current = args.version_file.read_text(encoding="utf-8").strip()
    bump_type = args.bump
    if bump_type == "auto":
        bump_type = select_bump_type(commit_messages(args.version_file, args.from_ref))

    updated = bump_version(current, bump_type)
    args.version_file.write_text(updated + "\n", encoding="utf-8")
    print(f"Bumped GPAD firmware version ({bump_type}): {current} -> {updated}")


if __name__ == "__main__":
    main()
