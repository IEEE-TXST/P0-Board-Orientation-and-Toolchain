# TXST IEEE Student Branch: FRDM-KL26Z Project Series
## Project Manual, P0: Board Orientation and Toolchain Setup

**Document status:** DRAFT v0.2, for project-leader review and bench testing before member use
**Track:** Embedded Systems | **Difficulty:** Beginner | **Sessions:** 1 (WS1, Sep 3, 2026)
**Applies to:** All 8 groups (24 members)
**Companion documents:** *IEEE TXST Project Developer Guideline* (standards this manual follows), *TXST IEEE FRDM-KL26Z Project Specification*, `SETUP_LOG.md` (verified OpenSDA bootloader procedure, referenced in Section 7)
**Demo code:** see `demo_code/` in this project's folder

---

## This Manual Is Split Into 4 Files

Long single files invite procrastination. Read only what you need, when you need it:

1. **`P0_1_Start_Here.md`** (this file) — how to use this manual, why P0 exists, purpose, prerequisites.
2. **`P0_2_Concepts_and_Hardware.md`** — background theory and the physical board tour. Read once if any term below is new to you.
3. **`P0_3_Setup_and_Walkthrough.md`** — the actual hands-on steps: install the toolchain, check the bootloader, flash the LED demo, write and flash UART hello world. **This is the file you follow during the session.**
4. **`P0_4_Reference.md`** — code structure explanation, sample output, session plan, milestones, debugging table, glossary, references, developer notes. Look things up here when stuck.

Section numbers (0-17) are kept consistent across all 4 files, so "see Section 8" always means the same section no matter which file you're in.

---

## How to Use This Manual

This manual is a reference, not required reading. The session itself is hands-on: flash the board, look at it, write some code, ask a leader when something breaks. Members should open these files when they hit something they don't understand, not read them top to bottom before starting. That's the whole point of writing them: so nobody is stuck waiting on a leader when the answer is already here.

Project leaders are the exception. Read all 4 files end to end and bench-test every step (Sections 8 to 10, in `P0_3_Setup_and_Walkthrough.md` and `P0_4_Reference.md`) on a physical board before WS1, so you can answer questions fast during the session instead of debugging live in front of the group.

This is written assuming **zero prior embedded experience**. If a term feels obvious to you, skip ahead; it's there for the member who has never done this before, which will be most of the room.

This is a separate document from the **Project Template** (the meeting-plan scaffold in Part 4 of the Guideline). Do not merge them.

**A note on accuracy:** every pin assignment, register name, and memory address in this manual was pulled directly from the installed `SDK_2_2_0_FRDM-KL26Z` source tree (board headers, pin-mux files, and linker scripts), not from general Kinetis knowledge, because the FRDM-KL26Z board revision does not share the same pinout as its KL25Z sibling. The one exception is the on-board ambient light sensor referenced in later projects; see the callout in Section 4.3 (`P0_2_Concepts_and_Hardware.md`).

---

## 0. Why This Session Exists

Every project this semester, P1 through the capstones, assumes you can already do four things: get code onto the board, know what's physically on the board, read output from it, and not panic when something doesn't work the first time. P0 is entirely about building those four muscles. There's no new sensor, no new algorithm here on purpose. If P0 goes well, every later project is "P0's workflow plus one new concept." If P0 is shaky, every later project starts with a fight against the tools instead of the actual engineering problem.

So don't rush this session. A member who deeply understands *why* dragging a file onto a fake USB drive programs a chip will debug P1's I2C issues faster than a member who memorized the steps without understanding them.

## 2. Purpose

By the end of this session, every member has: a working MCUXpresso IDE and SDK install, a board running a pre-built LED blink demo flashed by their own hand, a working mental model of what's physically on the board and where, and a UART "hello world" they wrote themselves, printing their name and a per-chip board ID, running on a serial terminal on their own laptop. This is the toolchain and confidence foundation for every project that follows (P1 to P3 and all four capstones).

No prior embedded experience is required. This is most members' first contact with bare-metal firmware (firmware with no operating system underneath it, which is what everything in this series is).

## 3. Prerequisites

None. This is the entry point for the whole series, including members who are concurrently taking the microprocessor course.

**Before Sep 3:** members must install MCUXpresso IDE and the Kinetis SDK (Section 5, in `P0_3_Setup_and_Walkthrough.md`); this is intro-meeting homework per the Fall 2026 schedule. Do not spend session time on installs if avoidable; use the first 10 minutes to fix installs that failed at home.

**Next:** if any term above is new to you, read `P0_2_Concepts_and_Hardware.md` next. Otherwise, skip straight to `P0_3_Setup_and_Walkthrough.md` and start doing.

---
*IEEE Texas State University Student Branch. Connect. Build. Inspire.*
