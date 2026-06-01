# GPAD firmware versioning

The GPAD firmware version is stored in `FIRMWARE_VERSION` as a Semantic Versioning
2.0.0 value. PlatformIO reads that file from `pre_extra_script.py` and exposes it
to the firmware as the `FIRMWARE_VERSION` macro.

When a pull request is raised against `main`, the `Assign GPAD firmware PR version`
workflow increments the patch component on that pull request branch and records the pull
request number as SemVer build metadata. For example, raising PR `#123` after `0.58.0`
produces `0.58.1+pr.123` in the PR itself, before it is merged.

Reopening the same pull request is idempotent: if its branch already records that PR
number, the workflow leaves the version unchanged. Version assignment is limited to
branches in this repository because GitHub does not grant the workflow permission to
push version commits to contributors' forks.

For an intentional release, update `FIRMWARE_VERSION` in a pull request to the
desired `MAJOR.MINOR.PATCH` baseline. The next raised pull request resumes patch
increments from that baseline.

The workflow pushes its isolated version commit to the open pull request branch, so it
does not require a direct push to `main`.
