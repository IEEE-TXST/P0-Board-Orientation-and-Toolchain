# P0: Setup and Walkthrough

*Part of the P0 manual. Start with `P0_1_Start_Here.md` if you haven't. This is the file to follow during the session. Section numbers match across all 4 files.*

---

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

> **This is a leader-side task, not a member task.** Do this ahead of WS1 for every board your group will use, so no member burns session time on a bootloader mismatch. `SETUP_LOG.md`'s troubleshooting table covers every failure mode we've hit doing this (wrong device node, un-synced mount, etc.). **On macOS**, flashing an already-updated board also needs a workaround for a `diskutil` mount issue; see `SETUP_LOG.md`'s "macOS Flashing Procedure" section.

## 8. Flashing the LED Blink Demo (Verified Working Example)

This is your first time actually doing what Section 6 described. Watch what happens to the board's drive contents and the LED as you go; understanding this loop (edit or obtain code, build, copy to drive, replug, observe) is 80% of embedded development.

The example used here is `SDK_2_2_0_FRDM-KL26Z/boards/frdmkl26z/driver_examples/gpio/led_output`. It's already been built and tested against this exact toolchain; a copy of the working binary and full source is in `demo_code/01_led_blink_demo/` in this project's folder.

**Step-by-step:**

1. Plug the board into the **SDA** port. A drive named `FRDM-KL26Z` should mount (not `MBED`; P&E Micro firmware names the drive after the board).
2. Copy `gpio_led_output.srec` (from `demo_code/01_led_blink_demo/`) onto that drive. This is the compiled program from Section 1's flash concept, already turned into the bytes the chip needs.
3. Wait for the copy to finish (`sync` if on macOS/Linux and the copy seems to hang; see Section 13, or SETUP_LOG.md if the drive won't mount at all on macOS).
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

### 8.1 Optional: Flashing and Debugging Straight From MCUXpresso IDE

The drag-and-drop method above is the default for a reason: it needs no extra setup and is what most members should use for a normal flash-and-run. This subsection is for when you specifically want the IDE's debugger (breakpoints, stepping through code line by line, inspecting variables while the program runs), since drag-and-drop alone can't do that.

**This setup has three real pitfalls that all look like the flash silently failing, worked out through actual hardware testing. Follow this exact path to skip them:**

1. **Don't use the toolbar's plain "Run" button.** MCUXpresso's SDK example projects build with `managedBuildOn="false"` (a project setting meaning the IDE just calls the external `make`/CMake build rather than tracking the output itself), so the IDE has no auto-generated launch target. Clicking "Run" without a real launch configuration either says **"Program not specified"**, or launches a `C/C++ Application` config that tries to run your ARM binary directly on your own laptop and instantly fails with **`exit value: 127`**. Either way, nothing was flashed to the board.
2. **Don't pick the LinkServer probe type.** This board's OpenSDA chip runs **P&E Micro's** firmware (Section 6, Section 7), not the CMSIS-DAP protocol LinkServer expects. Choosing LinkServer connects to a debug server, but its `ProbeList` command reports **"No probes found"** even with the board correctly plugged in, because it's speaking the wrong protocol to it entirely.
3. **The PEmicro launch config needs its Device field set explicitly, or it fails with `Illegal Device Type ()`.** MCUXpresso doesn't infer this from your project.

**Correct steps:**

1. **Run > Debug Configurations…**
2. Right-click **GDB PEMicro Interface Debugging**, choose **New Configuration**.
3. On the **Main** tab: set **Project** to your project (e.g. `01_led_blink_demo`), and **C/C++ Application** to `armgcc/debug/<your_output_name>.elf` (browse to it; it's the same `.elf` the build produces, just not converted to `.srec` yet).
4. Click the **PEmicro Debugger** tab. Find the **Device** field (it starts blank) and set it explicitly: for this board's chip (MKL26Z128VLH4), select **`KL26Z128M4`** from the device list (P&E's device database drops the leading "M" and package suffix from NXP's part number).
5. Click **Apply**, then **Debug**.
6. Watch the Console: a working flash shows `Erasing.` -> `Programmed.` -> `Checksum Verification Successful.` -> `Target has been RESET and is active.` (If the chip's flash already matches what you're about to load, it'll instead say `Application verified in memory. No need to reprogram.` and just reset, which is equally correct.)

Once this configuration exists, reuse it (select it by name in the Debug Configurations list and click Debug) instead of the toolbar buttons, which can spawn fresh blank configs that hit pitfall 1 or 3 again.

### 8.2 Exercise: Change the LED Color

Section 8's demo blinks the **red** LED. Before moving to Section 9, try changing it to blink **green** or **blue** instead. It's a small change, but doing it yourself is worth more than reading about it: it's the fastest way to actually feel a rule that governs every GPIO pin on this chip, not just this LED.

**Four things to know before you touch anything, so nothing here feels like it's happening to you instead of because of you:**

- This exercise only touches files inside `demo_code/01_led_blink_demo/`. It's a self-contained copy, not shared code. You cannot break Section 9 or any later project by editing it, and there's nothing to undo afterward unless you want to.
- You will not be calling any new function yourself. `BOARD_InitPins()` already runs automatically, once, before your blink loop ever starts (Section 9 will show you exactly where that call happens). You're only adding lines *inside* a function that's already being called, not wiring up something new.
- If, partway through, you get a compiler error naming something like `PIN31_IDX` as undefined, that is not a sign you did something wrong. It means the constant for that pin number doesn't exist in this file yet, you'll need to add a one-line `#define` for it, matching the style of the one already there for pin 29. This is expected, not a mistake to be anxious about.
- You do **not** need to worry about "on" versus "off" logic changing between colors. `board.h` defines all three colors the same way (driving the pin low turns that color on), and the loop code just flips whatever pin it's given, so the on/off behavior you already saw with red carries over automatically.

**How this actually works, so you're not just guessing at file names:**

A microcontroller pin is a shared, reusable piece of physical hardware, this is what the Glossary (Section 15) calls a **pin mux**: short for "multiplexer," a hardware switch that decides which one job a shared pin is doing right now. The same physical leg labeled "PTE29" on this chip could carry a GPIO signal, a UART signal, an ADC reading, or a timer output, one at a time, depending on what you tell it to be. Three separate things all have to line up before writing code that toggles an LED actually moves voltage on that pin:

1. **The port's clock must be on.** Every GPIO port (A through E) has its own clock, and most start **off** at power-up to save energy. A peripheral with no clock is electrically asleep, no matter what your code writes to it. This is `CLOCK_EnableClock(...)`, and it happens once, near the top of `BOARD_InitPins()` in `pin_mux.c`.
2. **The pin's mux must be set to GPIO.** This is the multiplexer switch from above, being set specifically to "GPIO." This chip defaults every pin to a specific function at reset, and it is usually *not* GPIO. This is `PORT_SetPinMux(..., kPORT_MuxAsGpio)`, also in `pin_mux.c`.
3. **The GPIO peripheral itself must be told "output," and your loop toggles it.** This is `GPIO_PinInit()` and `GPIO_TogglePinsOutput()`, both already written for you in `gpio_led_output.c`. This part doesn't change between colors, the same generic code drives whichever pin the `#define`s at the top point at.

Section 8's red LED already had steps 1 and 2 done for you, quietly, before you ever looked at the code. This exercise asks you to do them yourself for a pin that doesn't have them yet, which is the only way to actually notice they exist.

**Hints, in order.** Open the next one only if the previous one didn't get you moving. Each hint tells you where to look, not what to type, you're always the one making the edit.

<details>
<summary>Hint 1: where does "red" get chosen?</summary>

Open `demo_code/01_led_blink_demo/gpio_led_output.c` and look right after the `#include` lines near the top. Two `#define` lines name which LED the rest of the file drives. Those names come from somewhere else, open `board.h` and search for the word `RED` to find where they're actually defined, then look just below it for what's defined for the other two colors.

</details>

<details>
<summary>Hint 2: changing that alone won't be enough</summary>

Go ahead and swap the two `#define`s to the green (or blue) equivalents from `board.h`, then rebuild and reflash exactly as you did in Section 8 (drag-and-drop) or Section 8.1 (IDE debugger). Watch closely: does anything happen at all? If not, that's expected, not a mistake. Step 3 above (`GPIO_TogglePinsOutput`) is running fine, on a pin that steps 1 and 2 were never done for. Your code has nothing wrong with it; the pin just isn't listening yet.

</details>

<details>
<summary>Hint 3: find where red's pin was told to listen</summary>

Open `pin_mux.c` and look inside `BOARD_InitPins()`. Find the line mentioning `PTE29` (that's red). It calls a function that sets that pin's mux to GPIO, step 2 from above. There is no equivalent line yet for green's pin (`PTE31`) or blue's pin (`PTD5`). That's the missing piece. Add one, following the exact same pattern, same function, just a different pin number. Look near the top of the file for how `PTE29`'s pin number was turned into a named constant (something ending in `_IDX`); you'll need one of those for your new pin too, since none exists yet for pin 31 or pin 5.

</details>

<details>
<summary>Hint 4: only needed if you're doing blue</summary>

Look at the very top of `BOARD_InitPins()`, at the `CLOCK_EnableClock(...)` lines, step 1 from above. Only two of the chip's five GPIO ports are turned on there, because that's all red's LED and the UART needed. Blue's pin lives on a port that isn't one of those two. If you added blue's mux line in Hint 3 but still see nothing, this is almost certainly why: the mux is set, but the port is still asleep.

</details>

If you've worked through the hints and still aren't sure, or just want to check your answer, expand below. This isn't required reading, try it yourself first.

<details>
<summary>Full answer: switching to green (click to expand)</summary>

In `gpio_led_output.c`:

```c
// #define BOARD_LED_GPIO BOARD_LED_RED_GPIO
// #define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN
#define BOARD_LED_GPIO BOARD_LED_GREEN_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_GREEN_GPIO_PIN
```

In `pin_mux.c`, inside `BOARD_InitPins()` (PORTE's clock is already enabled for the red LED, so only the mux line is new, this is step 2 only, no step 1 needed):

```c
#define PIN31_IDX 31u   /*!< Pin number for pin 31 in a port */
...
PORT_SetPinMux(PORTE, PIN29_IDX, kPORT_MuxAsGpio);   /* PORTE29 (pin 17) is configured as PTE29 */
PORT_SetPinMux(PORTE, PIN31_IDX, kPORT_MuxAsGpio);   /* PORTE31 is configured as PTE31 */
```

**Heads up:** on the board this manual was tested on, green flashed and verified correctly (checksum matched, chip reset cleanly) but was hard to see under normal room light, see Section 17's Developer Notes. Don't assume it failed if nothing seems to happen; check in a dimmer spot before concluding something's wrong.

</details>

<details>
<summary>Full answer: switching to blue instead (click to expand)</summary>

Blue is on a different port (`PTD5`, not `PTE`), and that port's clock isn't enabled anywhere in this project yet, so this needs step 1 *and* step 2, one more line than green did.

In `gpio_led_output.c`:

```c
// #define BOARD_LED_GPIO BOARD_LED_RED_GPIO
// #define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN
#define BOARD_LED_GPIO BOARD_LED_BLUE_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_BLUE_GPIO_PIN
```

In `pin_mux.c`, inside `BOARD_InitPins()`:

```c
#define PIN5_IDX 5u   /*!< Pin number for pin 5 in a port */
...
CLOCK_EnableClock(kCLOCK_PortD);   /* Port D Clock Gate Control: Clock enabled (step 1) */
...
PORT_SetPinMux(PORTD, PIN5_IDX, kPORT_MuxAsGpio);   /* PORTD5 is configured as PTD5 (step 2) */
```

Blue was confirmed clearly visible on the board this manual was tested on.

</details>

Once you've made your change, rebuild and reflash the same way as Section 8 (or Section 8.1, if you're using the IDE's debugger).

## 9. Writing and Flashing a UART "Hello World" From Scratch

This is the session's real deliverable: not copying a demo, but writing a small program from the SDK's building blocks, so you understand every line instead of having memorized a recipe. Try this yourself before opening `demo_code/02_uart_hello_world_reference/`; that folder exists for when you're stuck, not as the starting point.

**Step-by-step:**

1. In MCUXpresso IDE, create a **new project** using the SDK wizard (**File > New > C/C++ Project**, or the Quickstart Panel's "Create a new C/C++ project..."), device = `MKL26Z128VLH4` (this board's actual chip, see Section 4.3), board files = **Default board files**, project type = **C Project**, SDK Debug Console = **UART**. This auto-generates `board.h`, `board.c`, `pin_mux.c`, and `clock_config.c`, the same board-support files every SDK example uses. You do not write these by hand; they're the "which physical pin does what" and "how fast does the chip run" configuration, generated once per board type. Leave the Components list at its defaults.
2. Open the generated `main.c` (named after your project, inside `source/`). Depending on your MCUXpresso version, it may already call the three standard init functions every SDK example needs, just possibly under newer names:
   ```c
   BOARD_InitBootPins();         // configures pins per pin_mux.c (Section 4.3's mapping)
   BOARD_InitBootClocks();       // starts the chip running at its normal 48 MHz speed
   BOARD_InitBootPeripherals();
   BOARD_InitDebugConsole();     // sets up UART0 at 115200-8-N-1 and connects it to PRINTF
   ```
   (Older SDK examples, including `01_led_blink_demo`, use `BOARD_InitPins()` / `BOARD_BootClockRUN()` instead. Same job, different generator version. If your `main.c` doesn't have these calls at all, add them yourself, in this order, before anything else in `main()`.) Every project this semester starts main() with some version of these calls. Once you've written this once, you'll recognize the pattern everywhere.
3. Read a real per-chip identifier to use as the "board ID." Every MKL26Z128 chip has an 80-bit number burned in at the factory that's unique to that specific physical chip (like a serial number), exposed as three read-only registers: `SIM->UIDMH`, `SIM->UIDML`, `SIM->UIDL` (there's no `UIDH` field on this smaller Kinetis part; that only exists on larger chips in the family). Print the low 32 bits as the board ID:
   ```c
   PRINTF("Member: <your name>\r\n");
   PRINTF("Board ID: %08X\r\n", SIM->UIDL);
   ```
   The point of using this instead of a made-up number: it proves the output is really coming from your specific board and not a copy-pasted screenshot.

   **Use `%08X`, not `%08lX`.** This matters more than it looks: a wizard-created project links against **Redlib** (NXP's own minimal C library) instead of the `newlib-nano` library `01_led_blink_demo`'s `armgcc` build uses, and Redlib's default `PRINTF` has `PRINTF_ADVANCED_ENABLE` off, which silently drops support for the `l` length modifier. With `%08lX`, the `l` doesn't get consumed as part of the format specifier, so it's treated as an unrecognized conversion character and `X` leaks out as a plain letter, printing something like `Board ID: lX` with no digits at all, no error, no warning. `SIM->UIDL` is already a 32-bit value and `unsigned long` is the same size as `unsigned int` on this chip anyway, so `%08X` alone prints the exact same result correctly. (Row 13 of Section 14 covers this too.)
4. Build. The output is a `.axf` file (this newer wizard's name for the same kind of file `01_led_blink_demo`'s `.elf` is, a full executable with debug info), typically under `Debug/`. Two ways to get it onto the board:
   - **IDE debugger (recommended here):** set up a Debug Configuration the same way as Section 8.1, pointing **C/C++ Application** at the `.axf`. Since this is a normal managed-build project (unlike the `armgcc`/CMake-based demos), MCUXpresso may auto-generate a working launch config for you, try Debug before manually creating one.
   - **Drag-and-drop:** a wizard project's default post-build steps often skip generating a `.bin`/`.srec`, check your build console for a commented-out `objcopy`/`checksum` line. If so, convert it yourself first: `arm-none-eabi-objcopy -O srec your_project.axf your_project.srec`, then flash that the same way as Section 8.
5. Open the serial terminal at 115200-8-N-1 on the board's COM/tty port (macOS/Linux: `ls /dev/tty.usb*` or `/dev/cu.usb*` to find it) **before** resetting the board, not after. `main()` typically prints this once near the top and never again, so if the terminal connects after the last reset, that message already went out unheard and won't repeat until the next reset. Once connected, trigger a fresh reset (the board's physical Reset button is cleanest, since it doesn't disturb the USB/debug connection at all) and confirm the message appears.

**Deliverable:** UART output on the member's own laptop terminal showing their name and their board's unique ID.

---

**Next:** `P0_4_Reference.md` for code structure, sample output, debugging, and the glossary.

---
*IEEE Texas State University Student Branch. Connect. Build. Inspire.*
