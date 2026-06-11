# GPAD firmware versioning

The GPAD firmware version is stored in `FIRMWARE_VERSION` as a plain Semantic
Versioning 2.0.0 value: `MAJOR.MINOR.PATCH`. PlatformIO reads that file from
`pre_extra_script.py` and exposes it to the firmware as the `FIRMWARE_VERSION`
macro.

Firmware versions do not include pull request numbers or build metadata. A
version like `0.58.5` is valid; a PR-derived version like `0.58.5+pr.123` should
not be used.

The version bump logic lives in the repository root at
`scripts/bump_gpad_firmware_version.py`. It can be run directly:

```sh
python3 scripts/bump_gpad_firmware_version.py
```

By default, the script chooses the SemVer component from commit messages since
the previous `FIRMWARE_VERSION` change:

- `BREAKING CHANGE:` or a Conventional Commit with `!` bumps `MAJOR`.
- `feat:` or `feature:` bumps `MINOR`.
- Any other change bumps `PATCH`.

You can override the automatic choice when preparing an intentional release:

```sh
python3 scripts/bump_gpad_firmware_version.py --bump minor
```

To enable automatic local bumping before pushes, install the repository hook
path once from the repository root:

```sh
git config core.hooksPath .githooks
```

The pre-push hook creates a separate `Bump GPAD firmware version` commit and
stops that first push. Re-run `git push` to send both your code and the version
commit. If `FIRMWARE_VERSION` is already part of the commits being pushed, the
hook lets the push continue without bumping again.

The GitHub workflow uses the same script for pull requests from branches in this
repository, so automated version commits also stay plain SemVer without PR
numbers.
