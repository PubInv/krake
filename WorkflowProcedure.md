
# Krake Workflow 

## Goals

* Every change is traceable to an issue.
* Small, reviewable branches.
* Clear validation (code or non-code).
* Works on both Windows and macOS.

 

## 0) One-time setup (Windows & macOS)

**Install Git**

* **Windows:** Install “Git for Windows” (use **Git Bash**), include **Git Credential Manager**.
* **macOS:** `xcode-select --install` (or `brew install git` if you use Homebrew).

**Configure Git**

```bash
git config --global pull.rebase true
git config --global fetch.prune true
git config --global init.defaultBranch main
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
```

**Line endings (important cross-platform)**

* **Windows (Git Bash):** `git config --global core.autocrlf true`
* **macOS/Linux:** `git config --global core.autocrlf input`

**Clone**

```bash
git clone https://github.com/PubInv/krake.git
cd krake
git remote -v
```

> Use **Terminal** (macOS) or **Git Bash** (Windows). VS Code’s built-in terminal works on both.

 
## Roles

* **Mentor:** creates & assigns issues, reviews PRs, **merges**.
* **Contributor (Volunteer):** works **only** on assigned issues, opens PR, updates per review.
* **Maintainer:** sets repo guidelines (branch protection, labels, templates, CODEOWNERS).

 -----


## 1) Issues 
* Mentors create & assign issues to contributors.
* Each issue includes:

  * **Goal & Acceptance criteria**
  * **Deliverables** (e.g., STL/STEP, schematic PDF/BOM diff/DRC, docs, scripts, code)
  * **Validation method** (how we verify—simulation, screenshots, photos, logs, ERC/DRC/print-fit, etc.)
* Contributors can create issues and assign them to mentor(s) at any time. However, the mentor or team lead should set priorities, and therefore should assign issues to contributors most of the time (contributors may complete obvious issues if it makes sense by assigning the issues to themselves). 

### Issue Criteria (for when a contributor is creating an issue)

issue [type]: Firmware, Enclosure, Documentation... 

## 2) Create a branch (from `main`)

Navigate to the issue in Github and use the user interface to create a branch, by scrolling the Development section.

<img width="351" height="92" alt="Image" src="https://github.com/user-attachments/assets/4e311c31-7da5-47de-b30c-38f3da2a526c" />

### Branch Naming
**Naming:** `type/issue-<number>-short-slug`
Types: `feature`, `bugfix`, `docs`, `cad`, `pcb`, `chore`, `refactor`

Then locally: 

```bash
git fetch origin
git checkout -b cad/issue-312-speaker-mount-v2 origin/cad/issue-312-speaker-mount-v2
```


## 3) Work locally  

Keep commits small and focused.

**Commit message style**

* For code: Conventional Commits
* For CAD/PCB/docs: plain but specific is fine

Examples:

```
cad(enclosure): add 2mm standoffs for DFPlayer (#312)
pcb(audio): reroute SPK +/- to reduce crosstalk (#287)
docs(factory-test): clarify rotary encoder checklist
```

**Artifacts & folders**

* Put source and **exported, viewable** artifacts together (e.g., PDF/PNG for schematics, STL/PNG for CAD).
* Typical folders: `firmware/`, `hardware/enclosure/`, `hardware/pcb/`, `docs/`.

Push often:

```bash
git add -A
git commit -m "cad(enclosure): add 2mm standoffs for DFPlayer (#312)"
git push
```

> Validation can be **anything appropriate**: screenshots, DRC/ERC reports, test photos, simulation results, print fit notes, logs, etc. We don’t assume a build/compile exists.

 

## 4) Open a Pull Request (Contributor)

On GitHub:

* **Title:** What changed + issue number
  e.g., “Enclosure: add 2 mm standoffs (Closes #312)”
* **Description must include:**

  * **What & why**
  * **Validation steps** (how to verify)
    *Examples:* “ERC/DRC screenshots attached”, “Print test part A—fits DFPlayer with 1.5 mm clearance”, “Photo of assembled prototype”, “Serial log shows expected event”
  * **Artifacts attached** (PNGs/PDFs/STLs/log snippets)
* **Link issue:** `Closes #<n>`
* **Reviewers:** your **mentor(s)** only
* **Labels:** match the issue (`cad`, `pcb`, `docs`, `firmware`, etc.)

**PR checklist (paste into PR):**

* [ ] Closes #<issue>
* [ ] Validation steps written & artifacts attached
* [ ] Only related files changed
* [ ] Correct folder structure

Use **Draft PR** if not ready.

 

## 5) Review & iterate

* Mentor reviews, requests changes or approves.
* Add follow-up commits; **don’t force-push** unless mentor asks.
* Resolve comments with “Fixed in `<short-hash>`” or rationale.

 
## 6) Merge (mentor-only)

* Only a **mentor** merges.
* **Strategy:** **Squash & merge** to keep history clean/complete.
* After merge: branch deleted, issue auto-closes via `Closes #<n>` (close manually if needed).

 
## 7) Sync after merge

```bash
git checkout main
git pull
git fetch --prune
```


## 8) Hotfixes (urgent)

* Branch from `main`: `hotfix/issue-<n>-slug`
* Same PR/review flow; mentor merges once validated.

---

 

## Branch protection for `main`

**What:** Rules that prevent direct pushes and enforce quality gates before merging.
 

* Restrict deletions
> Only allow users with bypass permissions to delete matching refs.

* Require a pull request before merging
> Require all commits be made to a non-target branch and submitted via a pull request before they can be merged.
> Require **1 approving reviews** (mentors)

* Block force pushes
> Prevent users with push access from force pushing to refs.

## PR Template: 


```
## What & Why
-

## Validation / How to Verify
1.
2.

## Artifacts (attach if relevant)
- [ ] Screenshots / PDFs / STLs
- [ ] Logs

## Links
Closes #<issue>

## Checklist
- [ ] Only related changes
- [ ] Folder structure respected
- [ ] Validation steps written
```
