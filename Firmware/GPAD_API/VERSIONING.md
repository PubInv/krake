# GPAD firmware versioning

The GPAD firmware version is stored in `FIRMWARE_VERSION` as a Semantic Versioning
2.0.0 value. PlatformIO reads that file from `pre_extra_script.py` and exposes it
to the firmware as the `FIRMWARE_VERSION` macro.

After a pull request is merged into `main`, the `Increment GPAD firmware version`
workflow increments the patch component and records the merged pull request
number as SemVer build metadata. For example, merging PR `#123` after `0.58.0`
produces `0.58.1+pr.123`.

Each merged pull request runs its own optimistic update. If multiple merges or a
direct update advance `main` at the same time, a failed push retries from the
latest branch state and increments that newer version. The workflow checks the
version-file commit history before changing anything, so rerunning an older PR's
workflow remains idempotent even after newer PRs have incremented the version.

For an intentional release, update `FIRMWARE_VERSION` in a pull request to the
desired `MAJOR.MINOR.PATCH` baseline. The next merged pull request resumes patch
increments from that baseline.

If `main` is protected against direct pushes, configure the repository rules so
this GitHub Actions workflow may push its isolated version commit. If that is
not allowed, the workflow intentionally fails after its retry limit instead of
silently leaving the firmware version stale.
