# Krake Documentation — First-Pass Diagnostic Analysis

**Sources reviewed:**
1. GitHub README.md (`PubInv/krake`)
2. Krake™ User/Operator Manual v0.1 (Google Doc)
3. Krake™ Developer's Technical Manual v0.1 (Google Doc)
4. HardwareX article draft (Overleaf, LaTeX)
5. Krake Test and Assembly Procedure Document v0.1.0 (Overleaf, LaTeX)

This is a diagnostic pass only — no rewriting yet. Goal is to surface conflicts, gaps, and structural
issues so we can agree on a fix order before touching prose.

---

## 1. Fact conflicts requiring a team decision

These are places where two or more documents state something different, and only the team can say
which is correct.

| Item | Source A | Source B | Notes |
|---|---|---|---|
| ~~**Physical dimensions**~~ **RESOLVED** | Developer Manual: "160mm x 110mm x 50mm (6"x4.25"x1.75"), weighs 232 grams" | HardwareX §7.3: "weighs less than 740 grams. Dimensions are: 150 x 180 x 90 mm" | **Not a documentation error — two real hardware states.** The 232g/160x110x50mm figure describes the **Rev1** enclosure with a 28mm speaker (too quiet, since retired). The Rev2 enclosure was enlarged to fit a 3" speaker for adequate volume, raising both size and mass. **Confirmed current Rev2 measurements (re-measured for parallax error): enclosure body 150mm x 180mm x 67mm, mass 742g; overall depth at the rotary encoder knob is 78mm.** HardwareX's "90mm" was an early rough estimate — supersede with 67mm (body) / 78mm (at knob). **Action:** Developer Manual should carry the Rev2 figures as current spec; the Rev1 232g figure should only remain if explicitly labeled historical/reference-only per the team's rev1 policy. |
| **Power draw** | Developer Manual: "9-12VDC... less than 500 mA and typically 250mA (less than 6 watts)" | Assembly Doc: bench supply 8–12V, current limit <1A, typical measured draw **150–190 mA** at 8.3–8.4V | Numbers are in the same ballpark but don't match. Assembly doc numbers are actual measurements from real units, so they're probably more trustworthy — Developer Manual reads like an earlier estimate. |
| **Firmware version** | README: firmware at **v0.50.0** | Developer Manual screenshot: `GPAD_API 0.19_20241119` | Big gap (0.19 vs 0.50.0). Developer Manual example is clearly stale and needs a refreshed screenshot/output. |
| ~~**PCB revision**~~ **RESOLVED** | Assembly Doc, page 1: "Confirm PCB is marked as **Rev 2.0**" | Same document, a few paragraphs later: "Confirm PCB is marked as **Rev 2.1**" (in both the standalone doc and the HardwareX-embedded version of the same section) | **Not a simple typo — a PCB-vs-PWA conflation.** Team convention going forward: **PCB Rev 2.0** = the bare board actually fabricated (design started ~Aug 2025, populated Jan 2026 batch). **PWA Rev 2.1** = PCB Rev 2.0 + enclosure + speaker + cable assembly + label — the complete assembled/tested unit. PCB Rev 2.1 (the in-progress silkscreen/BOM refinement) has **not been released** (no Gerbers generated) and should not appear as if describing built hardware. **Action:** every "Rev 2.1" reference to the built batch becomes "PWA Rev 2.1 (PCB Rev 2.0)" on first use per section, with a short disambiguation note near the top of the Assembly doc and the HardwareX Build Instructions section defining PCB vs. PWA. |
| **Unit serial range tested** | Assembly Doc body: example measurements for **US0001–US0005** | HardwareX §1: batch described as **US0006–US00015, LB0006–LB00011, LDN0001–LDN0005** | Different unit ranges in what's meant to be the same batch/procedure. Worth confirming which numbers are real. |
| **Powered TP102 (Vin) reading** | Assembly Doc §5.6.2 (Krake Rev 2.0 section): "TP102 Vin ≈ 5.04–5.06 V" | Same doc, later "Rev 2.1" powered-test table: "TP102 ≈ 4.5V" | Inconsistent within the same document; also 4.5V is a strange reading for a Vin rail expected to sit near the bench-supply voltage (8.3V) minus drop — worth double-checking this isn't a transcription error. |
| **Ordering price** | Developer Manual: "approximately $200USD," contact Robert Read directly | Not mentioned in README or HardwareX | Confirm this is still current before it goes in a permanent, indexed HardwareX article. |
| **OSHWA certification UID** | HardwareX specs table: `US002818` | Not mentioned in README, User Manual, or Dev Manual | Should probably be surfaced in the README and User Manual too, since it's a credibility signal for new visitors. |
| **Trademark usage** | User/Dev Manuals: consistent "Krake™" | README / HardwareX: plain "Krake," no ™ | Needs a house-style decision (see §3). |

---

## 2. Structural / completion gaps

**HardwareX article** — this is the least finished document and has the most template debris:
- The Abstract section still contains **unresolved editor/reviewer comment text** bleeding into the visible body (e.g., stray fragments like "not in template," "graphical abstract," mid-sentence interruptions). This reads as corrupted or improperly-resolved tracked changes/comments from Overleaf/Word round-tripping, and needs a clean rewrite pass before anything else in that document is touched.
- Several section headers still contain the **unmodified HardwareX instructional text** ("Write a short description...", "Add 3–5 bullet points...") mixed in with actual content — a sign these sections were drafted directly into the template without deleting the prompts.
- **Ethics statement, CRediT statement, Acknowledgements, References, "Useful References," Design Files table costs** are explicitly marked TBD, placeholder, or contain obvious placeholder values (e.g., BOM costs listed as `1000000` for the enclosure and PCB — clearly a formatting artifact, not real costs).
- §7 (Validation) has three subsections (Sound/Light, Electrical Parameters, Physical Parameters) that are almost entirely TBD, tagged to specific people ("TBD: Lee," "TBD: Lee, Courtney").
- The reference to "the Krake, the Zetron SentriMax" comparison seems to have an editor comment ("Missing is a comparison...") embedded directly in the body text.

**Developer Manual:**
- Multiple `TBD` and `????` placeholders in the Wireless Connection section (default password, exact button to hold for WiFi Manager, etc.) — this is core setup instructions for a real user, so these gaps block someone from actually using the device.
- "Communicating with the Krake" section literally says "TODO: Write this section in reference to the diagram below" — the diagram exists but the explanatory text was never written.
- Python MQTT example section says "TO BE DONE: NAGHAM ADD PYTHON CODE HERE."
- SPI Interface connector type is marked "TBD."
- Note this manual is dated/versioned as v0.1 — it may be the least mature of the four, other than the HardwareX draft.

**User Manual:**
- Shares roughly 70% of its content verbatim with the Developer Manual (Wireless Connection, Mute Button, Rotary Encoder, Ordering sections are nearly identical). This isn't necessarily wrong — some overlap is expected since they share an audience for some topics — but right now it looks like duplication-by-copy-paste rather than an intentional shared-content strategy. Worth deciding: cross-reference instead of duplicate, or keep parallel but ensure they never drift again.
- Same `????` and TBD gaps in the WiFi Manager section as the Developer Manual (since it was copied from there).

**Assembly & Test Procedure Document:**
- Most complete and "real" of all the docs — it has actual measured data, not just placeholders.
- Still carries **ISO template scaffolding** at the top (Background, Results/Discussion, References, Appendix headers) that are unfilled and read as boilerplate rather than a finished document.
- Revision history table still has `YYYY-MM-DD [Author Name]` placeholder rows for v0.2.0 and v1.0.0.
- Ends abruptly with "Ethics statements / CRediT / Acknowledgements / References — To be completed."
- Internally, this document appears **twice** in slightly different forms — once as a standalone procedure and once embedded (with more detail and more figures) inside the HardwareX draft's Build Instructions section. These have already started to drift from each other (see the Rev 2.0/2.1 and TP102 conflicts above), which is a preview of what will keep happening if they're maintained as two separate copies.

**README:**
- Most current and complete-feeling document. Good candidate as the "source of truth" for high-level facts (firmware version, feature list, licensing) that other docs should defer to or link to rather than restate.

---

## 3. Voice, terminology, and style drift

- **Trademark symbol**: User/Developer Manuals use "Krake™" consistently; README and HardwareX draft do not. Pick one house style — likely "Krake™" on first mention per document, plain "Krake" thereafter, matching normal trademark convention — and apply everywhere.
- **Point of view**: Manuals mix "we," "the Alarm Developer," "you," and third-person "the Krake" within the same paragraph. HardwareX draft is more consistently third-person/passive, as academic style expects. You've said you want a "we" team voice — that's very workable for the manuals and README, but note HardwareX has its own required conventions (more formal, methods-paper register) that shouldn't be fully "we"-ified in the same way — it already uses "we" in places, which is normal for HardwareX articles, so this is really just about consistency, not a wholesale voice change.
- **Product name capitalization/quoting**: "Krake™," "Krake," and "the Krake" (with article) are all used interchangeably, sometimes in the same sentence. Worth a simple style rule (e.g., always "the Krake" when used as a common noun, "Krake™" only on first mention in a document).
- **Author voice fingerprints**: A few passages read as clearly personal/first-person aside rather than team voice, e.g., Developer Manual: *"Lee thinks that the card should NOT be removable without disassembly..."* — this is useful institutional knowledge but needs to be converted to team voice and a stated decision, not attributed to one person by first name in a public document.
- **Redundant/parallel content**: the "Krake™ Purpose," Mute Button, Rotary Encoder, Wireless Connection, and Ordering sections are near-duplicates across User Manual and Developer Manual — same content, tiny wording differences. This is the clearest sign of multiple contributors solving the same problem independently.

---

## 4. What I'd recommend as the fix order

1. **Resolve the flagged fact conflicts** (§1) with the team — I can't guess these; they need a person who has the physical hardware in hand or the git history of the specs.
2. **Fix the HardwareX draft's structural debris** (stray comment text, unresolved template instructions) — this document is closest to being reviewed by an external editor and currently wouldn't pass a first read.
3. **Decide the User Manual / Developer Manual overlap strategy** — merge-and-cross-reference vs. intentional duplication — before doing any voice/style pass, since it changes how much rewriting is needed.
4. **Do the "we" voice and terminology pass** last, once facts and structure are settled, so we're not polishing prose that's about to change.

## 5a. Backlog — related but out of scope for this documentation pass

- **Schematic/KiCad project has no PWA-level revision field** — only PCB revision is currently tracked in the design files. Now that a PCB/PWA dual-numbering convention exists (see §1, PCB revision), it would be worth adding a PWA revision annotation directly to the schematic or project metadata so this doesn't have to be reconstructed from memory or documentation archaeology next time.
  - **Additional complicating history (for whoever picks up this ticket):** the schematic version was incremented to 2.1 around the same time PWA changes began, and a 2.1 PCB was created with silkscreen changes — but no footprint or copper changes are believed to have been made at the PCB level. Separately, the KiCad project files were corrupted at some point and had to be reverted to an earlier commit; as of this writing, both the schematic and PCB show as **Rev 2.0** again. There may be intermediate commits that preserve the 2.1 changes, but this has not yet been checked. **Not in the critical path for the HardwareX article** — noted here so it isn't lost, but doesn't block the documentation work.
  - Suggested as a GitHub issue for after the documentation work.

## 5. What I need from you to proceed

- Decisions on the six items in §1 (or confirmation that some are fine as-is / intentionally different states of the device).
- A call on the User/Developer Manual duplication strategy.
- Confirmation of fix order, or reprioritization if you'd rather start with the HardwareX article since it may be closer to a submission deadline.

Once you've weighed in, I can start on whichever document you want to tackle first — my suggestion would be the HardwareX article's Abstract and structural cleanup, since that's blocking the most.
