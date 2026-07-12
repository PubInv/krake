# Krake Documentation — Second-Pass Diagnostic Analysis

> **Relationship to pass 1:** this is a direct continuation of `krake-docs-diagnostic-pass1.md` (see that file's permalink in the team's GitHub issue for the original). It incorporates: the switch from the abandoned LaTeX Test and Assembly Procedure Document to the current GitHub-native Markdown version, all fact-conflict resolutions reached since pass 1 (PWA/enclosure dimensions and mass, PCB Rev 2.0 / PWA Rev 2.1 naming convention, journal name confirmation), and new findings surfaced by reviewing the current Markdown Assembly doc directly. Items marked **RESOLVED** below were open questions in pass 1 and are now settled; items still open are unchanged or newly added.

> **Correction incorporated in this pass:** the original pass 1 analyzed the LaTeX/Overleaf version of the Test and Assembly Procedure Document, which the team has since confirmed was **abandoned** in favor of a GitHub-native Markdown document. This pass replaces that source and re-checks every finding that touched it. The old LaTeX doc should not be treated as authoritative for anything going forward.

**Sources reviewed:**
1. GitHub README.md (`PubInv/krake`)
2. Krake™ User/Operator Manual v0.1 (Google Doc)
3. Krake™ Developer's Technical Manual v0.1 (Google Doc)
4. HardwareX article draft (Overleaf, LaTeX)
5. **Krake Test and Assembly Procedure Document v0.2.0** — `PWA_REV2/Krake_Test_and_Assembly_Procedure.md` on GitHub (`main` branch) — ~~formerly the Overleaf LaTeX version, now abandoned~~

This is a diagnostic pass only — no rewriting yet. Goal is to surface conflicts, gaps, and structural
issues so we can agree on a fix order before touching prose.

---

## 1. Fact conflicts requiring a team decision

These are places where two or more documents state something different, and only the team can say
which is correct.

| Item | Source A | Source B | Notes |
|---|---|---|---|
| ~~**Physical dimensions**~~ **RESOLVED** | Developer Manual: "160mm x 110mm x 50mm (6"x4.25"x1.75"), weighs 232 grams" | HardwareX §7.3: "weighs less than 740 grams. Dimensions are: 150 x 180 x 90 mm" | **Not a documentation error — two real hardware states.** The 232g/160x110x50mm figure describes the **Rev1** enclosure with a 28mm speaker (too quiet, since retired). The Rev2 enclosure was enlarged to fit a 3" speaker for adequate volume, raising both size and mass. **Confirmed current Rev2 measurements (re-measured for parallax error): enclosure body 150mm x 180mm x 67mm, mass 742g; overall depth at the rotary encoder knob is 78mm.** HardwareX's "90mm" was an early rough estimate — supersede with 67mm (body) / 78mm (at knob). **Action:** Developer Manual should carry the Rev2 figures as current spec; the Rev1 232g figure should only remain if explicitly labeled historical/reference-only per the team's rev1 policy. |
| ~~**Power draw**~~ **RESOLVED — not a conflict, different purposes** | Developer/User Manuals: "9-12VDC... less than 500 mA and typically 250mA (less than 6 watts)" — this is a **purchasing spec** for the external wall-wart power supply an end user or integrator needs to buy. | Assembly Doc: bench supply set to 8–12V, current limit <1A, typical measured draw **150–190 mA** at 8.3–8.4V — this is a **test-bench setup spec**, deliberately run near the minimum voltage the onboard regulators need, accounting for regulator headroom and the drop across the reverse-polarity protection diode. | **Not a discrepancy to reconcile — two different documents answering two different questions**, so the numbers don't need to match and shouldn't be forced to. **New action item:** the Assembly doc's bench-supply setup step doesn't currently explain *why* the minimum test voltage is set where it is. Worth adding a short explanation there (regulator headroom + reverse-polarity diode drop) so a technician understands the rationale, not just the number — this also protects against someone "helpfully" changing the test voltage in a later revision without understanding why it's set near the floor. |
| **Firmware version** | README: firmware at **v0.50.0** | Developer Manual screenshot: `GPAD_API 0.19_20241119` | Big gap (0.19 vs 0.50.0). Developer Manual example is clearly stale and needs a refreshed screenshot/output. |
| ~~**PCB revision**~~ **RESOLVED, and partially already fixed** | Assembly Doc §4 (Definitions) **already defines PCB vs. PWA correctly**: "PCB: ...to which electronic components are NOT yet attached" / "PWA: ...a PCB with components soldered/attached." Good — this matches the team's agreed convention below, no work needed there. | **However, the inconsistency in applying that distinction persists in the current Markdown doc**, not just the old LaTeX one: the "Unpacking" section says "Confirm PCB is marked as **Rev 2.0**," while "Incoming Sub-assembly Inspection" (a few lines later, same doc) says "Confirm PCB is marked as **Rev 2.1**." The top-level doc title is also still "Krake: PCB Rev 2.1," and body text says "This article focuses on the **Rev 2.1 PCB**..." — which directly contradicts the agreed PCB 2.0 / PWA 2.1 convention. | **Team convention (confirmed):** **PCB Rev 2.0** = the bare board actually fabricated (design started ~Aug 2025, populated Jan 2026 batch). **PWA Rev 2.1** = PCB Rev 2.0 + enclosure + speaker + cable assembly + label — the complete assembled/tested unit. PCB Rev 2.1 (in-progress silkscreen/BOM refinement) has **not been released** and should not appear as describing built hardware. **Action, still open:** the Assembly doc's title, its "This article focuses on the Rev 2.1 PCB..." line, and the "Confirm PCB is marked as Rev 2.1" instruction all need to change to reflect PCB 2.0 / PWA 2.1 — the definitions are already in the doc, they're just not applied consistently yet. Same fix needed in the HardwareX draft's build section (see new item below on HardwareX build-section staleness). |
| **Unit serial range tested** | HardwareX §1: batch described as **US0006–US00015, LB0006–LB00011, LDN0001–LDN0005** | The current Markdown Assembly doc **does not state a specific serial range** in the sections reviewed — this appears to have dropped out rather than contradicted. | No longer a cross-document conflict, but still an open question: does the HardwareX-stated batch range reflect the units actually referenced by the current procedure doc? Worth confirming before submission. |
| **Powered TP102 (Vin) reading** | Confirmed still present, unchanged, in the current Markdown doc's powered-test results table: "TP102 | ~4.5V" | The 8.3–8.4V bench supply and other readings suggest ~4.5V is anomalously low for this rail. | Not a copy artifact from the abandoned LaTeX doc — this is a live number in the current, authoritative procedure. Worth double-checking with whoever ran that test batch. |
| **Author list (Assembly/Test doc)** | Current Markdown doc lists **three authors**: Forrest Lee Erickson, Nagham Kheir, and **Courtney Ludick** (added since the abandoned LaTeX version, which listed only Erickson and Kheir) | HardwareX draft lists **six authors**: Nagham Kheir, Robert L. Read, Forrest "Lee" Erickson, Courtney Ludick, Juhandré Knoetze, Yhya Ayman | Closer alignment than before, but still not identical — confirm whether the Assembly doc's list is meant to be a subset (contributors to that specific document) before finalizing the CRediT statement. |
| **Internal date typo (new finding)** | Assembly doc's ISO title page block: "Date: **2027 - 02 - 08**" | Same document's Revision History table: v0.2.0 dated **2026-02-08** | Almost certainly a typo (2027 vs 2026) in the ISO title page block. Easy fix, worth catching before publication. |
| **Ordering price** | Developer Manual: "approximately $200USD," contact Robert Read directly | Not mentioned in README or HardwareX | Confirm this is still current before it goes in a permanent, indexed HardwareX article. |
| **OSHWA certification UID** | HardwareX specs table: `US002818` | Not mentioned in README, User Manual, or Dev Manual | Should probably be surfaced in the README and User Manual too, since it's a credibility signal for new visitors. |
| **Trademark usage** | User/Dev Manuals: consistent "Krake™" | README / HardwareX: plain "Krake," no ™ | Needs a house-style decision (see §3). |

---

## 2. Structural / completion gaps

**New finding (missing content, not a conflict): VESA mount and mounting orientation are undocumented.**
The enclosure has a **75×75mm VESA MIS-D mount** feature on the back (referenced only implicitly, as "VESA and Speaker Mount" threaded inserts, in the Assembly doc's Enclosure Assembly Procedure — never explained as a user-facing feature anywhere). None of the User Manual, Developer Manual, README, or HardwareX draft mention it. This is a real gap, not a drafting error, and it has a usability/safety dimension:
- The existing "Enclosure / Device Mounting" sections (User Manual, Developer Manual) describe a central mounting hole, side picture-hanging holes, and zip-tie flanges — but never the VESA pattern, and never state that the Krake is designed for **vertical surface mounting only**.
- Cables exit from the panel a user would likely perceive as the "bottom" (the side perpendicular to the LCD/front face) — which is not an obvious consequence of the design and isn't called out anywhere.
- **Action:** add a mounting-orientation **safety/best-practice note** — not just a spec line — to the User Manual and Developer Manual mounting sections: name the **75×75mm VESA MIS-D** pattern explicitly, state that the Krake is designed for vertical surface mounting only, and warn against setting the unit flat on a horizontal surface, since cables exit from that face and the enclosure isn't designed to bear load or rest stably on it.

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

**Assembly & Test Procedure Document (now on `main`, `PWA_REV2/Krake_Test_and_Assembly_Procedure.md`):**
- **This is a materially more advanced document than what was in the abandoned LaTeX version**, and more advanced than what's reflected in the HardwareX draft's embedded build section. New content not present anywhere else includes:
  - A full **Factory Test Procedure** with a structured hex-menu test suite ([0]–[F]: Power/ID, Inputs, LCD, LEDs/Lamps, DFPlayer, SD, Speaker, Wi-Fi AP, Wi-Fi STA, LittleFS, UART, SPI loopback, RS-232 loopback, **ElegantOTA**, Mute Button+LED) — this supersedes the old, much simpler "enter 4-5-6 into the serial monitor" test flow.
  - A complete **Enclosure Assembly Procedure** (3D-printed part list, threaded-insert installation, full step-by-step mechanical assembly) that doesn't exist in the HardwareX draft or the old LaTeX doc at all.
  - Real BOM costs (e.g., PCB $35/5, subassembly $217/5) replacing the old placeholder values (`1000000`) — the HardwareX draft's BOM table should be checked against this and updated, since it still likely has the placeholder figures.
  - A firmware update workflow via **ElegantOTA** (`.bin` upload) replacing the old direct-flash-only description.
  - Reference to a live GitHub Issue (#274) for firmware library specifications — a good sign of the team already using issues for exactly this kind of drift control.
  - A third author, Courtney Ludick, credited on the document (see fact-conflict table above).
- Still carries **ISO template scaffolding** at the top (Background, Results/Discussion, References, Appendix headers) that remain unfilled — same issue as before, not yet resolved by the update.
- Revision history table still has a `YYYY-MM-DD [Author Name]` placeholder row for the future v1.0.0 "Finalized for publication" entry.
- Ends abruptly with "Ethics statements / CRediT / Acknowledgements / References — To be completed" — same as before.
- **Still has the PCB Rev 2.0/2.1 inconsistency** discussed above, even in this updated version — the definitions in §4 are correct, but the applied labels in the body aren't consistent with them yet.
- **New, higher-priority finding: the HardwareX draft's embedded Build Instructions section is now significantly more out of date than previously assessed.** It was already showing signs of drift from the old LaTeX Assembly doc; now that the real procedure has moved to a substantially expanded, GitHub-native document, the gap is much larger — entire sections (Factory Test menu, Enclosure Assembly, ElegantOTA workflow) exist only in the Markdown doc and aren't reflected in the HardwareX draft at all. **Recommendation: when we get to the HardwareX Build Instructions section, treat it as needing significant new content pulled in from the current Markdown doc, not just a terminology/label fix.**

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

1. **Resolve the flagged fact conflicts** (§1) with the team — I can't guess these; they need a person who has the physical hardware in hand or the git history of the specs. *(Several already resolved through this conversation — dimensions/weight, PCB/PWA convention, journal name.)*
2. **Fix the HardwareX draft's structural debris** (stray comment text, unresolved template instructions) — this document is closest to being reviewed by an external editor and currently wouldn't pass a first read.
3. **Import current content into the HardwareX Build Instructions section from the authoritative Markdown Assembly doc** — this is now a bigger job than originally scoped. The HardwareX draft's build section reflects an old, abandoned version of the procedure; the Factory Test menu, Enclosure Assembly steps, and ElegantOTA firmware workflow all need to come across, not just get relabeled.
4. **Decide the User Manual / Developer Manual overlap strategy** — merge-and-cross-reference vs. intentional duplication — before doing any voice/style pass, since it changes how much rewriting is needed.
5. **Do the "we" voice and terminology pass** last, once facts and structure are settled, so we're not polishing prose that's about to change.

## 5a. Backlog — related but out of scope for this documentation pass

- **Schematic/KiCad project has no PWA-level revision field** — only PCB revision is currently tracked in the design files. Now that a PCB/PWA dual-numbering convention exists (see §1, PCB revision), it would be worth adding a PWA revision annotation directly to the schematic or project metadata so this doesn't have to be reconstructed from memory or documentation archaeology next time.
  - **Additional complicating history (for whoever picks up this ticket):** the schematic version was incremented to 2.1 around the same time PWA changes began, and a 2.1 PCB was created with silkscreen changes — but no footprint or copper changes are believed to have been made at the PCB level. Separately, the KiCad project files were corrupted at some point and had to be reverted to an earlier commit; as of this writing, both the schematic and PCB show as **Rev 2.0** again. There may be intermediate commits that preserve the 2.1 changes, but this has not yet been checked. **Not in the critical path for the HardwareX article** — noted here so it isn't lost, but doesn't block the documentation work.
  - Suggested as a GitHub issue for after the documentation work.

- **Product naming strategy for multi-use Krake hardware (new, strategic — flagged by team, not a documentation defect).** The Krake hardware platform is general-purpose; firmware developed so far targets the alarm-annunciator use case, but the team has discussed a second application: using the same hardware as a controller/HMI for PolyVent (Public Invention's open-source ventilator, which currently has no user interface and is laptop-controlled only). No name yet exists for this second application.
  - **Recommendation discussed:** decouple the hardware platform identity ("Krake") from the identity of each firmware-defined application built on it. Keep "Krake" as the name of the physical hardware/enclosure/PCB platform (as already documented), and give the PolyVent-controller application its own distinct product name and documentation trail, explicitly described as "built on the Krake hardware platform" rather than as a mode or variant of Krake itself.
  - **Why this matters more than a typical naming choice:** unlike the alarm-annunciator use case, a PolyVent controller is a component of respiratory life-support equipment. Medical device regulatory frameworks (e.g., FDA 510(k)) generally treat a device with a different intended use as a different device for regulatory purposes — same hardware, different firmware/intended use, means separate classification, labeling, and documentation requirements are likely down the line. Establishing separate product identity now, before firmware exists, avoids having to retroactively disentangle "Krake" (general-purpose, no medical claims) from a future medical-context product sharing its name.
  - **Not in the critical path for the HardwareX article** — the HardwareX submission concerns the alarm-annunciator hardware/firmware as it exists today, and doesn't need to resolve this naming question. Noted here so it isn't lost, and so it's on record before any PolyVent-controller firmware or documentation work begins.
  - Suggested next step: loop in whoever manages PolyVent's regulatory/quality strategy early, since the naming decision is small relative to the eventual compliance path but easier to get right from the start than to fix later.

## 5. What I need from you to proceed

- Decisions on the six items in §1 (or confirmation that some are fine as-is / intentionally different states of the device).
- A call on the User/Developer Manual duplication strategy.
- Confirmation of fix order, or reprioritization if you'd rather start with the HardwareX article since it may be closer to a submission deadline.

Once you've weighed in, I can start on whichever document you want to tackle first — my suggestion would be the HardwareX article's Abstract and structural cleanup, since that's blocking the most.
