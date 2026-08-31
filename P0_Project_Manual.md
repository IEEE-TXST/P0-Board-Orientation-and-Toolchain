# TXST IEEE Student Branch: FRDM-KL26Z Project Series
## Project Manual, P0: Board Orientation and Toolchain Setup

**Document status:** DRAFT v0.2, for project-leader review and bench testing before member use
**Track:** Embedded Systems | **Difficulty:** Beginner | **Sessions:** 1 (WS1, Sep 3, 2026)
**Applies to:** All 8 groups (24 members)
**Companion documents:** *IEEE TXST Project Developer Guideline* (standards this manual follows), *TXST IEEE FRDM-KL26Z Project Specification*, `SETUP_LOG.md` (verified OpenSDA bootloader procedure, referenced in Section 7)
**Demo code:** see `demo_code/` in this project's folder

---

## How to Use This Manual

This manual is a reference, not required reading. The session itself is hands-on: flash the board, look at it, write some code, ask a leader when something breaks. Members should open this document when they hit something they don't understand, not read it top to bottom before starting. That's the whole point of writing it: so nobody is stuck waiting on a leader when the answer is already here.

Project leaders are the exception. Read this end to end and bench-test every step (Sections 8 to 10) on a physical board before WS1, so you can answer questions fast during the session instead of debugging live in front of the group.

This is written assuming **zero prior embedded experience**. If a term feels obvious to you, skip ahead; it's there for the member who has never done this before, which will be most of the room.

This is a separate document from the **Project Template** (the meeting-plan scaffold in Part 4 of the Guideline). Do not merge them.

**A note on accuracy:** every pin assignment, register name, and memory address in this manual was pulled directly from the installed `SDK_2_2_0_FRDM-KL26Z` source tree (board headers, pin-mux files, and linker scripts), not from general Kinetis knowledge, because the FRDM-KL26Z board revision does not share the same pinout as its KL25Z sibling. The one exception is the on-board ambient light sensor referenced in later projects; see the callout in Section 4.3.

---

## 0. Why This Session Exists

Every project this semester, P1 through the capstones, assumes you can already do four things: get code onto the board, know what's physically on the board, read output from it, and not panic when something doesn't work the first time. P0 is entirely about building those four muscles. There's no new sensor, no new algorithm here on purpose. If P0 goes well, every later project is "P0's workflow plus one new concept." If P0 is shaky, every later project starts with a fight against the tools instead of the actual engineering problem.

So don't rush this session. A member who deeply understands *why* dragging a file onto a fake USB drive programs a chip will debug P1's I2C issues faster than a member who memorized the steps without understanding them.

## 1. Concepts You Need Before Starting

If any of this is new to you, read it once now. It will make every later section make sense instead of feeling like a list of magic incantations.

**Microcontroller vs. development board.** The actual "computer" here is a single chip called the **MKL26Z128**, about the size of a fingernail, soldered onto the board. Everything else on the board (USB connectors, LEDs, buttons, sensors) exists to let you talk to that one chip and let it talk back to you. When people say "the board," they usually mean this whole assembly; when they say "the MCU" (microcontroller unit) or "the target," they mean specifically that one chip.

**Firmware and flashing.** A microcontroller has no operating system, no hard drive, and no "Save As" dialog. Your program lives in a small region of permanent memory built into the chip called **flash memory**, the same physical concept as a USB flash drive, and it starts running that program automatically the instant power is applied. "Flashing" means writing your compiled program into that flash memory. There's no install process, no double-clicking an icon; the chip simply always runs whatever is currently sitting in its flash. This is why unplugging and replugging the board is enough to "run" a new program: the moment power returns, the chip starts executing what's in flash from address zero.

**Why you need a whole toolchain just to blink an LED.** Your C code is text. The chip only understands raw binary machine instructions for its specific processor (an ARM Cortex-M0+ here). Turning your text into something the chip can run takes several tools working together: a **compiler** (translates C into machine instructions), a **linker** (decides where in flash and RAM everything goes, using a linker script), and a **programmer/flasher** (actually writes the result into the chip). The **IDE** (MCUXpresso, in our case) is the application that bundles all of this together with an editor so you don't run each tool by hand from a terminal. The **SDK** (software development kit) is NXP's library of pre-written code for this chip's peripherals (GPIO, UART, I2C, ...) so you're not writing register-level code from day one.

**OpenSDA: a second, invisible computer that programs the first one.** Here's the part that trips people up: this board actually has *two* microcontrollers on it. One is the MKL26Z128, your target, the chip you're programming. The other is a smaller chip (the MK20DX128) whose only job is to sit between your laptop's USB port and the target chip, translating "a file was copied to a fake USB drive" into "write this binary into the target's flash memory." This translator setup is called **OpenSDA**. You never write code for it; it just needs to be running the correct version of its own firmware (Section 6 explains why, and how to check).

**GPIO.** General-Purpose Input/Output. The simplest thing a microcontroller pin can do: read whether a voltage is present (input, e.g. a button) or set a voltage present or absent (output, e.g. an LED). Every project this semester eventually reduces to some combination of GPIO plus a specific peripheral.

**UART and the serial terminal.** UART (Universal Asynchronous Receiver/Transmitter) is a simple two-wire protocol for sending text one byte at a time between two devices. It's how the board talks back to you: your program calls something like `PRINTF("hello\r\n")`, and those characters travel over UART, through the OpenSDA chip, and appear as text in a terminal program on your laptop (PuTTY, Tera Term, etc.), exactly like a command-line window. Without a working UART connection, your board is a black box; you can't see what it's thinking. This is why P0 ends with getting UART output working: it's the debugging lifeline for the rest of the semester.

**Flash vs. RAM, in plain terms.** Flash memory is permanent (survives power loss) but slow to write and limited in how many times it can be rewritten over its lifetime; it holds your program. RAM (SRAM here) is fast and freely rewritable but empties completely the instant power is lost; it holds your program's variables while it's running. You'll see both mentioned in Section 4.2.

Keep these seven ideas in mind and the rest of this manual should read as mechanics, not mystery.

## 2. Purpose

By the end of this session, every member has: a working MCUXpresso IDE and SDK install, a board running a pre-built LED blink demo flashed by their own hand, a working mental model of what's physically on the board and where, and a UART "hello world" they wrote themselves, printing their name and a per-chip board ID, running on a serial terminal on their own laptop. This is the toolchain and confidence foundation for every project that follows (P1 to P3 and all four capstones).

No prior embedded experience is required. This is most members' first contact with bare-metal firmware (firmware with no operating system underneath it, which is what everything in this series is).

## 3. Prerequisites

None. This is the entry point for the whole series, including members who are concurrently taking the microprocessor course.

**Before Sep 3:** members must install MCUXpresso IDE and the Kinetis SDK (Section 5); this is intro-meeting homework per the Fall 2026 schedule. Do not spend session time on installs if avoidable; use the first 10 minutes to fix installs that failed at home.

## 4. Hardware Overview

### 4.1 MCU and Debug Chip

Recall from Section 1: there are two chips on this board. This table makes that concrete.

| Item | Detail |
|---|---|
| Target MCU | NXP **MKL26Z128VLH4**: ARM Cortex-M0+, 48 MHz core, 128 KB flash, 16 KB SRAM. This is the chip that runs your code. |
| Debug MCU | **MK20DX128** running P&E Micro OpenSDA v1 firmware. This is the translator chip; you never program this one directly. |
| Programming interface | USB Mass Storage Device (MSD) drag-and-drop of `.srec` files, no separate debug probe needed. This is what makes "copy a file to a drive" actually flash the target chip. |
| Two USB ports on board | **SDA** port (labeled on silkscreen, meaning it's printed on the board itself) is programming/debug, and talks to the OpenSDA chip. **USB** port is target USB (used starting Capstone 4B, USB HID), and talks directly to the target chip. They are physically identical connectors; read the silkscreen, don't guess. |

The MKL26Z128 has no floating-point unit (hardware for doing math with decimal numbers quickly). Every project in this series that touches signal math (P3's FIR filter) uses fixed-point arithmetic (representing fractional numbers as scaled whole numbers) specifically because this chip has no hardware shortcut for it.

### 4.2 Memory Map

A "memory map" is just a list of which address ranges mean what to the chip. You won't need to memorize this for P0, but it explains *why* flashing works the way it does, and P2/P3 will require you to understand it directly. Pulled straight from the SDK's linker script (`MKL26Z128xxx4_flash.ld`), not estimated:

| Region | Address Range | Size | Contents |
|---|---|---|---|
| Interrupt vector table | `0x0000_0000` to `0x0000_01FF` | 512 B | The very first thing the chip reads at power-on: where to start running code, and where to jump for each type of interrupt. This is why your program always "just starts," there's a fixed, known address the chip always looks at first. |
| Flash configuration field | `0x0000_0400` to `0x0000_040F` | 16 B | Backdoor key and flash protection bytes. **Never overwrite this**; a bad value can permanently lock the chip so it can never be reprogrammed. The SDK's default project templates already set this correctly; you'd have to go out of your way to break it. |
| Application code (flash) | `0x0000_0410` to `0x0001_FFFF` | ~127 KB | Your compiled program lives here, permanently, until you flash something new over it. |
| SRAM | `0x1FFF_F000` to `0x2000_2FFF` | 16 KB | Split into SRAM_L (below `0x2000_0000`) and SRAM_U (above); the linker and startup code handle this split, you don't need to manage it directly. This is where your variables live while the program runs, and it's wiped every time power is removed. |

Peripheral registers (the actual on/off switches for things like GPIO pins and UART) live in a separate fixed address range (`0x4000_0000` and up). The SDK's driver functions (`GPIO_PinInit()`, `UART_Init()`, etc.) handle reading and writing those addresses for you, so you will not need to touch raw addresses in P0. You'll do it directly starting in P2 (SysTick and PendSV) and P3 (DMA), which is intentional: P0 to P1 teach you the SDK's abstractions, then P2 to P3 peel them back.

### 4.3 On-Board Peripherals and Pin Reference

"Peripheral" just means any built-in hardware feature beyond the bare processor: an LED, a button, a sensor, a communication port. Each one is wired to specific physical pins on the chip, listed here.

| Peripheral | Pin(s) | Notes |
|---|---|---|
| RGB LED | Red = **PTE29**, Green = **PTE31**, Blue = **PTD5** | Each color is a separate GPIO-driven LED sharing one plastic package, so it looks like one LED but is really three you can control independently. Active-low: driving the pin low (0 volts) turns that color on, which is the opposite of what most beginners expect. |
| Push button SW1 | **PTC3** | Interrupt-capable (`PORTC_PORTD_IRQn`, meaning the chip can react instantly when it's pressed instead of having to constantly check). Used from P1 onward. |
| Debug UART (console) | RX = **PTA1**, TX = **PTA2** | This is **UART0**, routed through the OpenSDA chip so it appears on your laptop as a single USB virtual serial port. Settings: 115200 baud (bits per second), 8 data bits, no parity, 1 stop bit, usually written as "115200-8-N-1." Every project in this series uses these same settings for console output unless stated otherwise. |
| Accelerometer (FXOS8700CQ, senses tilt/motion and magnetic field on 6 axes) | SCL = **PTE24**, SDA = **PTE25** (I2C0) | I2C is a two-wire protocol for talking to small sensor chips. This sensor's address on that bus is one of `0x1C`, `0x1D`, `0x1E`, or `0x1F` depending on how the board was manufactured. The SDK's I2C example plays it safe and probes all four, confirming it found the right one by reading a `WHO_AM_I` register (a kind of "who are you" question every I2C sensor should answer; this one replies `0xC7`). Don't hardcode one address; probe for it. Used starting P1. |
| Capacitive touch slider (TSI) | Electrode 1 = **PTB16** (TSI0_CH9), Electrode 2 = **PTB17** (TSI0_CH10) | Two touch-sensitive pads whose combined signal tells you roughly where along the slider your finger is, not a single on/off button. Used starting P1. |

> **Verify before the session: ambient light sensor.** The project spec lists an on-board ambient light sensor among P0's peripheral tour, and it's referenced again in Capstone 4D. No ambient-light driver, demo, or board macro exists anywhere in the installed `SDK_2_2_0_FRDM-KL26Z` tree, and the standard FRDM-KL26Z bill of materials (same physical board as the FRDM-KL25Z) is not known to include a dedicated ambient-light chip. Before WS1, a project leader should physically inspect a board against the schematic (NXP `SCH-29319` or the *FRDM-KL26Z User's Guide*) and confirm what's actually there. If there is no dedicated sensor, the practical substitute already supported by the SDK is the **ADC16 driver example** (`driver_examples/adc16/`), reading a photoresistor or potentiometer wired to a header analog pin; this is what P1's "samples an analog value via ADC" and P4-D's "light sensor" exercises can fall back to. **Do not present an unverified ambient light sensor as fact to members.** Flag it as unconfirmed until a leader has physically checked a board.

### 4.4 Physical Board Tour (talking points for the walkthrough)

Use this as the script for the "walk through schematic callouts" portion of WS1. Point at each item on a physical board as you go, and say what it's for, not just its name; members remember "this is how you'll talk to the accelerometer in P1" far better than "this is I2C."

1. **SDA USB connector**, nearest the board edge, labeled `SDA`. This is the only port used for programming today, and it's the one that talks to the OpenSDA chip described in Section 1.
2. **USB target connector**, the second, identical-looking USB connector. Not used in P0; used starting with Capstone 4B (USB HID), where the target chip itself becomes a USB device.
3. **Reset button**, used only to enter OpenSDA bootloader mode (Section 7); not needed for normal flashing.
4. **RGB LED**, small tri-color package near the center of the board. This is what the blink demo drives.
5. **SW1 push button**, labeled on silkscreen. Used starting P1.
6. **Accelerometer (FXOS8700CQ)**, small IC on the underside or top side of the board depending on revision. Point out that it talks over the two I2C pins already broken out on the headers, so you don't need extra wiring to use it in P1.
7. **Capacitive touch slider**, the two gold pads with a serpentine (wavy) trace, usually along one edge.
8. **Expansion headers (J1/J2/J9/J10)**, the pin rows around the board perimeter. This is where P3's potentiometer and P4-D's HC-05 module will connect; nothing plugs in for P0, but knowing where they are now saves time later.

## 5. Toolchain Setup

### 5.1 Install MCUXpresso IDE

Download from NXP (free registration required; a `.edu` email works fine, no purchase). Install for your OS (Windows, macOS, and Linux are all supported). This is the IDE used for every project in the series; install it once, correctly, now. Remember from Section 1: this one application bundles the editor, compiler, and linker you need, so you're not assembling a toolchain by hand.

### 5.2 Install the Kinetis SDK for FRDM-KL26Z

This chapter uses **SDK 2.2.0**, already present in this repository at `SDK_2_2_0_FRDM-KL26Z/`. Recall from Section 1: the SDK is NXP's pre-written driver code for this chip's peripherals, so you write `UART_Init(...)` instead of poking raw registers. In MCUXpresso IDE, use the **Installed SDKs** view (bottom-left panel) and drag the SDK zip or folder in, or use **File > Import > General > Existing Projects into Workspace** and point at an example under `SDK_2_2_0_FRDM-KL26Z/boards/frdmkl26z/` to confirm the SDK resolves correctly.

### 5.3 Install a Serial Terminal

**PuTTY** (Windows), **Tera Term** (Windows), or `screen`/`minicom` (macOS/Linux): any terminal that can open a serial port at a fixed baud rate works. This is the window where the UART output described in Section 1 will actually show up. Confirm it's installed before Sep 3; you cannot see any UART output without it, and a member with a working board but no terminal will think their code failed when it actually didn't.

## 6. What OpenSDA Is (in more detail)

Section 1 introduced OpenSDA as a "translator chip." Here's the mechanism: the MK20DX128 runs its own small program that watches for two things. First, it presents itself to your laptop as an ordinary USB flash drive; when you drag a `.srec` file (a text-based file format that encodes your compiled program plus the addresses it belongs at) onto that drive, OpenSDA reads it and writes the corresponding bytes into the target chip's flash memory over an internal debug connection you never see. Second, it forwards UART traffic between the target chip's TX/RX pins and a virtual serial port on your laptop, which is how PRINTF output reaches your terminal.

No JTAG/SWD probe (the more traditional, separate hardware debugger used in industry) and no driver install are needed for basic flashing (Windows may prompt for a driver on first plug-in; accept the default). This drag-and-drop approach is simpler than what you'll see in a real job, which is exactly why it's the right teaching platform for P0: it removes a layer of setup complexity so the session can focus on the concepts in Section 1 rather than debugger configuration.

## 7. Checking and Updating the OpenSDA Bootloader

The **bootloader** is the specific part of OpenSDA's firmware that handles the drag-and-drop flashing described in Section 6. Like any firmware, it has versions, and an older version had a bug (detailed in `SETUP_LOG.md`) that this chapter has already worked around. Most chapter boards should already be updated. **Check this once per board, before flashing anything:**

1. Plug the board into the **SDA** port normally (no reset held).
2. A drive should mount. Look for a file `SDA_INFO.HTM` on it and check the `BOOTVER` field.
3. If `BOOTVER` reads `1.11` or higher, skip to Section 8.
4. If it reads `1.09`, the bootloader needs a one-time update before this board can be used. **Full verified procedure is in `SETUP_LOG.md`** (Steps 1 to 3): hold Reset, plug into SDA port, remount the drive with the `sync` option, copy `BOOTUPDATEAPP_Pemicro_v111.SDA` onto it, unplug and replug (no reset), and confirm `BOOTVER` now reads `1.11`.

> **This is a leader-side task, not a member task.** Do this ahead of WS1 for every board your group will use, so no member burns session time on a bootloader mismatch. `SETUP_LOG.md`'s troubleshooting table covers every failure mode we've hit doing this (wrong device node, un-synced mount, etc.).

## 8. Flashing the LED Blink Demo (Verified Working Example)

This is your first time actually doing what Section 6 described. Watch what happens to the board's drive contents and the LED as you go; understanding this loop (edit or obtain code, build, copy to drive, replug, observe) is 80% of embedded development.

The example used here is `SDK_2_2_0_FRDM-KL26Z/boards/frdmkl26z/driver_examples/gpio/led_output`. It's already been built and tested against this exact toolchain; a copy of the working binary and full source is in `demo_code/01_led_blink_demo/` in this project's folder.

**Step-by-step:**

1. Plug the board into the **SDA** port. A drive named `FRDM-KL26Z` should mount (not `MBED`; P&E Micro firmware names the drive after the board).
2. Copy `gpio_led_output.srec` (from `demo_code/01_led_blink_demo/`) onto that drive. This is the compiled program from Section 1's flash concept, already turned into the bytes the chip needs.
3. Wait for the copy to finish (`sync` if on macOS/Linux and the copy seems to hang; see Section 13).
4. Unplug and replug the board normally. The firmware runs immediately; no reset needed, because (per Section 1) the chip always runs whatever is currently in flash the moment power returns.
5. **Expected result:** the red LED toggles on and off roughly twice per second.

**What's actually in that program**, so this isn't a black box: it configures one pin as a GPIO output, then loops forever, waiting a bit and flipping that pin's voltage each time. Section 9's code walkthrough covers this line by line.

**To build it yourself from source** (useful once you want to modify it):

```bash
cd demo_code/01_led_blink_demo/armgcc
export ARMGCC_DIR=$(dirname $(dirname $(which arm-none-eabi-gcc)))
cmake -DCMAKE_TOOLCHAIN_FILE="../../../../../../tools/cmake_toolchain_files/armgcc.cmake" \
      -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug .
make -j4
arm-none-eabi-objcopy -O srec debug/gpio_led_output.elf gpio_led_output.srec
```

(That last line converts the compiler's native output format, `.elf`, into the `.srec` format OpenSDA's bootloader actually accepts. This is the "linker" step from Section 1 made concrete.)

Members using MCUXpresso IDE instead of the command line: import via **File > Import > C/C++ > Existing Code as Makefile Project**, or open the example project directly if the SDK was imported per Section 5.2, then **Build** and drag the resulting `.srec` from the `Debug/` output folder onto the board's drive.

## 9. Writing and Flashing a UART "Hello World" From Scratch

This is the session's real deliverable: not copying a demo, but writing a small program from the SDK's building blocks, so you understand every line instead of having memorized a recipe. Try this yourself before opening `demo_code/02_uart_hello_world_reference/`; that folder exists for when you're stuck, not as the starting point.

**Step-by-step:**

1. In MCUXpresso IDE, create a **new project** using the SDK wizard, board = `frdmkl26z`. This auto-generates `board.h`, `board.c`, `pin_mux.c`, and `clock_config.c`, the same board-support files every SDK example uses. You do not write these by hand; they're the "which physical pin does what" and "how fast does the chip run" configuration, generated once per board type.
2. In `main.c`, call the three standard init functions every SDK example calls first:
   ```c
   BOARD_InitPins();        // configures pins per pin_mux.c (Section 4.3's mapping)
   BOARD_BootClockRUN();    // starts the chip running at its normal 48 MHz speed
   BOARD_InitDebugConsole(); // sets up UART0 at 115200-8-N-1 and connects it to PRINTF
   ```
   Every project this semester starts main() with some version of these three calls. Once you've written this once, you'll recognize the pattern everywhere.
3. Read a real per-chip identifier to use as the "board ID." Every MKL26Z128 chip has an 80-bit number burned in at the factory that's unique to that specific physical chip (like a serial number), exposed as three read-only registers: `SIM->UIDMH`, `SIM->UIDML`, `SIM->UIDL` (there's no `UIDH` field on this smaller Kinetis part; that only exists on larger chips in the family). Print the low 32 bits as the board ID:
   ```c
   PRINTF("Member: <your name>\r\n");
   PRINTF("Board ID: %08lX\r\n", SIM->UIDL);
   ```
   The point of using this instead of a made-up number: it proves the output is really coming from your specific board and not a copy-pasted screenshot.
4. Build, then flash the resulting `.srec` the same way as Section 8 (drag-and-drop onto the `FRDM-KL26Z` drive).
5. Open the serial terminal at 115200-8-N-1 on the board's COM/tty port and confirm the message appears.

**Deliverable:** UART output on the member's own laptop terminal showing their name and their board's unique ID.

## 10. Code Structure Explanation

| File | What It Does |
|---|---|
| `board.h` / `board.c` | Board-specific macros (`BOARD_LED_RED_GPIO_PIN`, `BOARD_DEBUG_UART_BASEADDR`, etc.) and the `BOARD_InitDebugConsole()` helper. Auto-generated by the SDK project wizard for the `frdmkl26z` board; don't hand-edit unless you know why. |
| `pin_mux.c` | Calls `PORT_SetPinMux()` for every pin your project uses, selecting which peripheral function (GPIO, UART, I2C, ...) each physical pin performs. This is why PTA1/PTA2 become UART0_RX/TX instead of plain GPIO; a pin can only do one job at a time, and this file is what decides which. |
| `clock_config.c` | Sets up the MCG (clock generator, the circuit that produces the chip's timing signal) so the core runs at 48 MHz and peripherals get correctly divided clocks. Generated by the Config Tools; you will not hand-write this until well past this series. |
| `gpio_led_output.c` (Section 8 example) | Configures one GPIO pin as output (`GPIO_PinInit`), then loops forever calling `GPIO_TogglePinsOutput()` with a software delay in between. This is the entire blink demo, no interrupts, no timers, just a busy-wait loop (code that wastes time on purpose by counting), which is exactly why P1 replaces the delay loop with a real timer, once you've seen why busy-waiting is wasteful. |
| Your `main.c` (Section 9) | The three `BOARD_Init*()` calls, then your own `PRINTF()` output. This pattern, init pins, init clock, init console, then application logic, repeats in literally every project in this series. |

## 11. Sample Output

Terminal, 115200-8-N-1, after flashing the Section 8 blink demo:

```
 GPIO Driver example

 The LED is taking turns to shine.
```

Terminal, after flashing a correct Section 9 "hello world":

```
Member: Jane Doe
Board ID: 1A2B3C4D
```

(The actual 8 hex digits will differ per board; that's the point, it's read from silicon, not hardcoded. If your output matches this pattern but with different digits and your own name, you did it correctly.)

## 12. Session Plan (maps to Guideline Section 4.6)

| Meeting | Phase | What Members Do | Deliverable | Slide Focus |
|---|---|---|---|---|
| 1 of 1 | Toolchain and board tour | Verify IDE/SDK install (fix any that failed at home). Flash the LED blink demo via MSD drag-and-drop (Section 8). Walk the physical board tour (Section 4.4) against schematic callouts. Write and flash a UART hello world from scratch (Section 9). | Board running blink demo, then UART printing member name and board ID. | Photo/video of terminal output; what broke and how it was fixed. Nothing more; no theory slides needed, this manual is the theory reference. |

## 13. Milestones and Success Criteria

| Milestone | Success Criteria | Evidence |
|---|---|---|
| Toolchain verified | MCUXpresso IDE opens, SDK example project builds without error | Screenshot of a clean build console |
| Blink demo flashed | Board's drive shows `FRDM-KL26Z`; LED visibly toggling after replug | Live demo / short video |
| Board tour complete | Member can point to and name each peripheral in Section 4.3 without notes, and say roughly what it's used for | Verbal check by leader |
| UART hello world (exit criteria) | Terminal shows member's name and an 8-hex-digit board ID pulled from `SIM->UIDL` | Terminal screenshot |

## 14. Project-Specific Debugging Reference

| # | Common Problem | Suggested Debugging Steps | Difficulty |
|---|---|---|---|
| 1 | Copied `.srec`/`.SDA` file, but board behavior doesn't change | The drive was not mounted with the `sync` option (macOS/Linux buffers writes and OpenSDA never sees them). Unmount and remount with `--options sync` (see `SETUP_LOG.md`'s "Key Rule"), then re-copy. | Easy |
| 2 | Drive shows as `BOOTLOADER` instead of `FRDM-KL26Z` after a normal replug | The application firmware wasn't flashed yet, or the board is stuck in bootloader mode. Check `SDA_INFO.HTM`'s `BOOTVER`; if the board only ever shows `BOOTLOADER`, redo Section 7/8 without holding Reset on the final replug. | Easy |
| 3 | No terminal output at all | Wrong COM/tty port selected, or wrong baud rate. Confirm 115200-8-N-1. On Windows, check Device Manager for the port number; on macOS/Linux, run `ls /dev/tty.usb*` or `/dev/ttyACM*` before and after plugging in to identify the new device. | Easy |
| 4 | Terminal shows garbage characters instead of text | Baud rate mismatch between the terminal and `BOARD_DEBUG_UART_BAUDRATE` (115200 in every P0 example). Fix the terminal's baud setting; the firmware side is correct if it matches Section 9 exactly. | Easy |
| 5 | Build fails with "board.h not found" or similar missing-header errors | The SDK wasn't correctly linked into the new project (Section 5.2 skipped or pointed at the wrong path). Re-import via **File > Import > Existing Projects into Workspace** from `SDK_2_2_0_FRDM-KL26Z`, confirm include paths in project properties. | Medium |
| 6 | `arm-none-eabi-objcopy: command not found` (command-line build path) | Toolchain not installed or not on `PATH`. Install `binutils-arm-none-eabi` (Linux) or the ARM GNU toolchain for your OS, confirm with `arm-none-eabi-gcc --version`. | Easy |
| 7 | `udisksctl: not a mountable filesystem` (Linux, after a bootloader update) | The v1.11 bootloader changes the device node from `/dev/sda` to `/dev/sda1` because the drive becomes partitioned. Use `/dev/sda1` for every command after the bootloader update. | Medium |
| 8 | Board never leaves bootloader mode no matter what's copied | Almost always a `sync`-mount miss, but if that's already correct, confirm you unplugged and replugged **without** holding Reset on the step that's supposed to boot the application; holding Reset always re-enters the bootloader. | Medium |

## 15. Glossary

- **Microcontroller (MCU):** a small computer on one chip, with its own processor, memory, and peripherals, and no operating system underneath it.
- **Firmware:** the program that runs directly on a microcontroller, permanently stored in its flash memory.
- **Flashing:** writing a compiled program into a chip's flash memory so it starts running that program at the next power-on.
- **OpenSDA:** P&E Micro's on-board debug/programming interface. A second MCU (here, MK20DX128) that exposes flash programming as a USB mass-storage drag-and-drop.
- **MSD:** Mass Storage Device, the USB class OpenSDA uses to appear as a drive.
- **.srec:** Motorola S-record, a text-based file format encoding a binary firmware image plus address information; what OpenSDA's MSD bootloader accepts.
- **SDK (Kinetis SDK v2):** NXP's peripheral driver library and board-support package. Provides `fsl_*.h` drivers (GPIO, UART, I2C, ...) so projects don't manipulate raw registers directly until P2/P3 require it.
- **Pin mux:** the hardware feature letting one physical pin serve several possible peripheral functions; `pin_mux.c` selects which function is active for each pin your project uses.
- **GPIO:** General-Purpose Input/Output; the simplest way a pin can read or set a voltage.
- **UART:** Universal Asynchronous Receiver/Transmitter, the serial peripheral used for all console/debug output in this series.
- **Baud rate:** how many bits per second a UART connection transfers; both sides must agree on this number or the output looks like garbage.
- **WHO_AM_I register:** a read-only identification register many I2C sensors expose (here, the FXOS8700CQ returns `0xC7`) so software can confirm it's talking to the expected device before trusting its data.
- **SIM (System Integration Module):** the Kinetis peripheral block holding chip-level configuration and identification registers, including the factory-programmed unique ID (`UIDMH`/`UIDML`/`UIDL`) used in Section 9.
- **Flash vs. RAM:** flash is permanent but slow to rewrite and holds your program; RAM is fast, freely rewritable, and holds your variables, but loses everything when power is removed.

## 16. References

- `SETUP_LOG.md` (this repository): verified OpenSDA bootloader update and MSD flashing procedure.
- `SDK_2_2_0_FRDM-KL26Z/` (this repository): SDK source, used as the ground truth for every pin/register value in this manual.
- NXP *FRDM-KL26Z User's Guide* and schematic (`SCH-29319` or current revision): for physical peripheral verification, especially the ambient light sensor question in Section 4.3.
- NXP *KL26 Sub-Family Reference Manual*: full register-level detail beyond what's needed for P0.
- FXOS8700CQ datasheet: accelerometer/magnetometer register map, referenced starting P1.

## 17. Developer Notes

- **Open item:** confirm or refute the ambient light sensor (Section 4.3) against physical hardware before WS1. This affects the accuracy of P0's peripheral tour and P4-D's capstone description; flag to chapter leadership either way so the spec doc can be corrected if needed.
- All pin/register values here were verified against the installed `SDK_2_2_0_FRDM-KL26Z` source, not assumed from general Kinetis/KL25Z familiarity. The FRDM-KL26Z board revision's LED pins (PTE29/PTE31/PTD5) differ from the commonly-cited KL25Z pinout, which is exactly the kind of error this verification step catches.
- Because this manual doubles as the leader's bench-test script, each of the five project leaders should run Sections 7 to 9 start-to-finish on a real board before Sep 3 and note anything that doesn't match here.
- Future revision idea: once Section 4.3's ambient light sensor question is resolved, fold the correct ADC pin/peripheral into this manual so P1 and P4-D can cite it directly instead of re-deriving it.

---
*IEEE Texas State University Student Branch. Connect. Build. Inspire.*
