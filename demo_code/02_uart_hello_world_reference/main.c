/*
 * P0 reference solution: UART "hello world" from scratch.
 *
 * This is what Section 9 of the P0 Project Manual (P0_3_Setup_and_Walkthrough.md)
 * walks through building. Prints a member name placeholder and a per-chip
 * board ID read from the MKL26Z128's factory-programmed unique ID register
 * (SIM->UIDL).
 *
 * Reference only. Write your own version first; check this only if stuck.
 */

/*
 * WHAT: Prints a name and a per-chip ID number over UART, once, at startup.
 *
 * HOW: Same three-stage shape as the LED demo (init board, then do the
 * "real" work), except here the "real" work is two PRINTF calls instead of
 * a blink loop, and the loop at the end just idles instead of doing
 * anything repeatedly.
 *
 * WHY: This is the first program in the series that reads real hardware
 * state (the chip's own factory-burned unique ID) rather than just writing
 * to a pin. It's also the first practical use of the debug console for
 * output you actually care about, not just a status message.
 */

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"

int main(void)
{
    /*
     * WHAT: One-time board bring-up, identical in purpose to the LED demo.
     * HOW: Pins, clock tree, then the debug console UART, in that order.
     * WHY: PRINTF() below won't produce any output at all if the console
     * isn't initialized first, and the console itself depends on the clock
     * tree already being configured, so the order here isn't arbitrary.
     */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /*
     * WHAT: Sends a plain text line identifying the member running this
     * board.
     * HOW: A literal placeholder string; you're expected to edit this to
     * your own name as part of Section 9.
     * WHY: Gives each board's serial output a human-readable label, useful
     * once multiple boards are being flashed and tested side by side during
     * a session.
     */
    PRINTF("Member: <your name here>\r\n");
    /*
     * WHAT: Prints a 32-bit hardware ID that's unique to this specific
     * physical chip.
     * HOW: SIM->UIDL reads one word of the chip's factory-programmed unique
     * ID out of the SIM (System Integration Module) peripheral's registers;
     * %08X formats it as 8 hex digits, zero-padded.
     * WHY: `%08X`, not `%08lX`, is deliberate: a wizard-created project
     * links against Redlib (NXP's minimal C library), and Redlib's default
     * PRINTF has advanced formatting disabled, which silently drops the `l`
     * length modifier instead of erroring. That turns `%08lX` into garbage
     * output (`lX` with no digits) with no warning at all. Since SIM->UIDL
     * is already 32 bits and unsigned long is the same width as unsigned
     * int on this chip, plain `%08X` prints the identical, correct value.
     * This exact gotcha is also documented as debugging-table Row 13 in
     * P0_4_Reference.md.
     */
    PRINTF("Board ID: %08X\r\n", SIM->UIDL);

    /*
     * WHAT: Idles forever after the two PRINTF calls above have run once.
     * HOW: An empty infinite loop, no delay, no work, nothing.
     * WHY: Unlike the LED demo, this program has nothing left to do after
     * printing its two lines, but main() still can't be allowed to return
     * (there's no caller to return to). An empty while(1) is the simplest
     * correct way to just stop here without doing anything further.
     */
    while (1)
    {
    }
}
