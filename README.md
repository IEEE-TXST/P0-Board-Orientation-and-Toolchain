# P0: Board Orientation and Toolchain Setup

*Everyone gets the board running and understands what they are working with.*

IEEE TXST Student Branch, Fall 2026 FRDM-KL26Z project series. Entry point for every member, including those with zero prior embedded experience.

**Platform:** NXP FRDM-KL26Z (ARM Cortex-M0+, 48 MHz, 128 KB Flash), MCUXpresso IDE, Kinetis SDK v2.

## What this project is

Members set up MCUXpresso IDE and the Kinetis SDK, flash a pre-built LED blink demo to confirm the board and toolchain work, tour the board's on-board hardware (RGB LED, accelerometer, capacitive touch slider, ambient light sensor, push button), and finish by writing and flashing a UART "hello world" from scratch. One session, no prior embedded background required.

## Exit criteria

Every member's board powers on, blinks, and prints a UART message they wrote themselves to a terminal on their laptop.

## Repo layout

- `P0_1_Start_Here.md`, `P0_2_Concepts_and_Hardware.md`, `P0_3_Setup_and_Walkthrough.md`, `P0_4_Reference.md`: the Project Manual, split into four files. Start with file 1. The manual is a fallback resource, not required reading; work through the session hands-on first and check the manual only if stuck.
- `demo_code/01_led_blink_demo/`: the verified LED blink demo used in Section 8. This is the one demo folder in the whole series meant to be built directly (its own `armgcc/` build), since Section 8.2's LED-color exercise needs a disposable sandbox.
- `demo_code/02_uart_hello_world_reference/`: reference solution for the UART hello world Section 9 has you build from scratch. Write your own version first.
- `SETUP_LOG.md`: verified OpenSDA bootloader update and firmware flashing procedure (Linux and macOS).

## Getting started

Read `P0_1_Start_Here.md` first, then follow `P0_3_Setup_and_Walkthrough.md` step by step.

## Resume line

Not applicable. P0 is toolchain orientation, not itself a resume-worthy technical deliverable; the first resume line in this series appears in [P1](https://github.com/IEEE-TXST/P1-Sensor-Dashboard).
