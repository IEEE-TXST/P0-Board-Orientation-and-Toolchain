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

/* Prints a name and a per-chip ID number over UART, once, at startup. Same
   init-then-work shape as the LED demo, except the "work" here is two
   PRINTF calls instead of a blink loop. This is the first program in the
   series that reads real hardware state (the chip's own factory-burned
   unique ID) instead of just writing to a pin. */

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"

int main(void)
{
    /* One-time board bring-up: pins, clock tree, then the debug console.
       PRINTF() below produces nothing if the console isn't initialized
       first, and the console depends on the clock tree already being
       configured, so this order isn't arbitrary. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Placeholder string, edit it to your own name as part of Section 9.
       Labels each board's serial output, useful once several boards are
       being flashed and tested side by side. */
    PRINTF("Member: <your name here>\r\n");
    /* SIM->UIDL reads one word of the chip's factory-programmed unique ID
       out of the SIM peripheral's registers; %08X formats it as 8 hex
       digits. Use %08X, not %08lX: a wizard-created project links Redlib,
       whose default PRINTF silently drops the `l` length modifier instead
       of erroring, turning %08lX into garbage output ("lX", no digits) with
       no warning. SIM->UIDL is already 32 bits and unsigned long is the
       same width as unsigned int on this chip, so plain %08X prints the
       identical, correct value. Also documented as debugging-table Row 13
       in P0_4_Reference.md. */
    PRINTF("Board ID: %08X\r\n", SIM->UIDL);

    /* Idles forever once the two PRINTF calls above have run. Unlike the
       LED demo there's nothing left to do, but main() still can't return
       (there's no caller to return to), so an empty while(1) is the
       simplest correct way to stop here. */
    while (1)
    {
    }
}
