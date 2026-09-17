# P0 Supplement: MCUXpresso Project Setup, a Screenshot Walkthrough

*This is a separate, standalone document, not one of the four split files (`P0_1_Start_Here.md` through `P0_4_Reference.md`). Those files describe this same setup in words; this one shows it happening, screen by screen, on this exact toolchain (MCUXpresso IDE v25.6, macOS). Use whichever format helps more. If a screenshot ever looks different from your own IDE, trust your own screen, minor version differences are normal.*

---

## What This Covers

There are two different ways to get a project open in MCUXpresso, and this project series actually uses both:

1. **Creating a brand-new project from scratch**, via the SDK wizard. This is what every project in the series (P1 through the capstones) has you do: one new project per session, then you copy logic in from `demo_code/` reference folders.
2. **Importing an existing folder as a project directly**, which is the one thing P0's `demo_code/01_led_blink_demo/` needs, since (as `P0_3_Setup_and_Walkthrough.md` Section 8 explains) that folder is the deliberate exception in this series kept independently buildable.

Both are shown below with real screenshots, in the order they actually happened.

---

## Part A: Creating a New Project From Scratch (the SDK Wizard)

This is the method you'll use for every project's own dashboard/scheduler/pipeline project, exactly as `P0_3_Setup_and_Walkthrough.md` Section 9 and every later project's manual describe.

### Step 1: Start the wizard

From the **Quickstart Panel** (bottom-left of the IDE), hover over **"Import SDK example(s)..."** and MCUXpresso shows what it actually does: **"Invoke the new SDK project wizard."** Click it.

![Quickstart Panel showing the "Invoke the new SDK project wizard" tooltip](demo_pictures/Screenshot%202026-09-16%20at%209.28.10%20PM.png)

*Don't be thrown by the label "Import SDK example(s)"; this is the same entry point the manual calls "the SDK wizard." There isn't a separate, more obviously-named button for "create a blank project."*

### Step 2: Pick the board

The wizard's first real page asks for a device or board. Select **KL2x** on the left, then click the **frdmkl26z** board that appears on the right (the tooltip confirms it: "FRDM-KL26Z: Freedom Development Platform for Kinetis KL16 and KL26 MCUs").

![SDK Wizard board selection page, FRDM-KL26Z highlighted](demo_pictures/Screenshot%202026-09-16%20at%209.28.20%20PM.png)

### Step 3: Configure the project

On the next page, name your project (this example uses `Demo_Project`; use something meaningful for your own, like `p1_sensor_dashboard`). Confirm:

- **Device Packages:** `MKL26Z128VLH4` (this board's real chip)
- **Board:** Default board files
- **Project Type:** C Project
- **SDK Debug Console:** UART

Then click **Finish**.

![SDK Wizard "Configure the project" page with project name, device, and UART console selected](demo_pictures/Screenshot%202026-09-16%20at%209.28.43%20PM.png)

### Step 4: See what got generated

The new project opens with a generated `main.c` already calling the standard boot sequence (`BOARD_InitBootPins()`, `BOARD_InitBootClocks()`, `BOARD_InitBootPeripherals()`, `BOARD_InitDebugConsole()`) and a placeholder `PRINTF("Hello World\r\n")`. This is the same three-or-four-call pattern every project in this series starts `main()` with.

![Newly generated Demo_Project.c showing the boot init calls and Hello World](demo_pictures/Screenshot%202026-09-16%20at%209.29.00%20PM.png)

### Step 5: Build it once, immediately

Before writing anything of your own, build the untouched wizard output first, so you know a clean baseline compiles. **Project > Build Project** (or the hammer icon). A successful build ends with `0 errors, 0 warnings`.

![Console showing "Build Finished. 0 errors, 0 warnings" for Demo_Project](demo_pictures/Screenshot%202026-09-16%20at%209.30.09%20PM.png)

If you see that line, the toolchain is working end to end and you're ready to start copying real logic in from the matching `demo_code/` reference folder.

---

## Part B: Importing an Existing Folder as a Project (the LED Demo Exception)

`demo_code/01_led_blink_demo/` is the one folder in this whole series meant to be built directly, not copied into a wizard project (see `P0_3_Setup_and_Walkthrough.md` Section 8). The manual's command-line path uses `cmake`/`make` directly; this section is the equivalent path entirely through the IDE's GUI, for members who'd rather not use a terminal.

### Step 1: Open the Import wizard

From the **File** menu, choose **Import...**.

![File menu with Import... highlighted](demo_pictures/Screenshot%202026-09-16%20at%209.34.22%20PM.png)

### Step 2: Choose "Existing Code as Makefile Project"

Under **C/C++**, select **Existing Code as Makefile Project**, not "C/C++ Project" and not the plain "New Project" options. This matters because `01_led_blink_demo/` is a real, pre-existing source folder with its own `armgcc/` build already in it, not something the wizard should generate fresh.

![Import wizard with "Existing Code as Makefile Project" selected](demo_pictures/Screenshot%202026-09-16%20at%209.34.29%20PM.png)

### Step 3: Point it at the demo folder

Browse to this project's `demo_code/` folder and select **01_led_blink_demo**.

![File picker inside demo_code/, with 01_led_blink_demo selected](demo_pictures/Screenshot%202026-09-16%20at%209.34.49%20PM.png)

### Step 4: Build it, and don't panic at the first console message

Once imported, the project shows up as `demo_project (in 01_led_blink_demo)` with its real files: `board.c/h`, `clock_config.c/h`, `gpio_led_output.c`, `pin_mux.c/h`, the pre-built `gpio_led_output.srec`, and `readme.txt`.

![Imported 01_led_blink_demo project tree and build console](demo_pictures/Screenshot%202026-09-16%20at%209.35.10%20PM.png)

**A real gotcha caught here, worth adding to your own troubleshooting notes:** the first build attempt in this console shows

```
You have not agreed to the Xcode license agreements. Please...
"make all" terminated with exit code 69. Build might be inc[omplete]
```

This is a macOS-specific issue, not a project or SDK problem: this Mac's command-line developer tools (which `make` and the ARM toolchain depend on) require Xcode's license to be accepted at least once, and nobody had done that yet on this machine. The fix is a one-time terminal command:

```bash
sudo xcodebuild -license accept
```

After accepting it, the exact same build succeeds cleanly, which is why the console in this screenshot also shows a second, later `Build Finished. 0 errors, 0 warnings` line right below the failure. If your very first build on a fresh Mac fails with `exit code 69` and a mention of Xcode licensing, this is why, and it's a one-time fix.

### Step 5: Confirm the build directory (optional, good to know)

If you ever need to check or change where this project's `make` actually runs, **Project > Properties > C/C++ Build** shows the **Build directory**, `${workspace_loc:/demo_project}/armgcc` for this import. You shouldn't need to change this for a normal import, but it's useful to know where to look if a build ever seems to be reading the wrong folder.

![Properties for demo_project, C/C++ Build page showing the armgcc build directory](demo_pictures/Screenshot%202026-09-16%20at%209.35.31%20PM.png)

### Step 6: Everyday builds from here

Once imported, use the **Project** menu for routine rebuilds: **Build All**, **Build Project**, or **Clean...** if you want to force everything to recompile from scratch (a clean build also ends with its own `0 errors, 0 warnings` line, just faster, since there's nothing to actually compile).

![Project menu open, with Clean-only build output showing 0 errors, 0 warnings](demo_pictures/Screenshot%202026-09-16%20at%2010.07.36%20PM.png)

The same **Properties > C/C++ Build** page is reachable from here too, shown full-size below for reference.

![Properties for demo_project, C/C++ Build page, full window](demo_pictures/Screenshot%202026-09-16%20at%2010.07.45%20PM.png)

---

## A Third Option You Might Notice: "Open Projects from File System..."

While exploring the **File** menu you may also come across **Open Projects from File System...**, a sibling of the **Import...** entry used above.

![File menu with "Open Projects from File System..." highlighted](demo_pictures/Screenshot%202026-09-16%20at%209.31.57%20PM.png)

This one is for a folder that **already contains real MCUXpresso/Eclipse project files** (a `.project` and `.cproject`), and just opens it directly instead of walking you through a wizard. The screenshot below shows this dialog pointed at a different project's demo folder purely to illustrate the picker itself, not as an instruction to build that folder this way.

![Import Projects from File System dialog, path field showing a demo_code folder](demo_pictures/Screenshot%202026-09-16%20at%209.32.17%20PM.png)

**Don't use this on most `demo_code/` folders in this series.** Per the project-wide build convention (`P0_3_Setup_and_Walkthrough.md` Section 8, and repeated in every later project's manual), `demo_code/` is reference-only, read and copy logic from it, never build it directly, with exactly three documented exceptions: this project's own `01_led_blink_demo/`, P4-B's USB gamepad reference (imported via **Import SDK example(s)...**, not this dialog), and P4-C's `app/` firmware. If a folder isn't one of those three, open a new wizard project (Part A above) and copy the code in instead.

---

**Related reading:** `P0_3_Setup_and_Walkthrough.md` Sections 8 to 9 (the same steps in words, plus the command-line alternative), `P0_4_Reference.md` Section 14 (the project's debugging table, a good place to add the Xcode license gotcha above if you want it indexed there too).

---
*IEEE Texas State University Student Branch. Connect. Build. Inspire.*
