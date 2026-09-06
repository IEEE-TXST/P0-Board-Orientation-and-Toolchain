# P0: Concepts and Hardware

*Part of the P0 manual. Start with `P0_1_Start_Here.md` if you haven't. Section numbers match across all 4 files.*

---

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

---

**Next:** `P0_3_Setup_and_Walkthrough.md` for the actual hands-on steps.

---
*IEEE Texas State University Student Branch. Connect. Build. Inspire.*
