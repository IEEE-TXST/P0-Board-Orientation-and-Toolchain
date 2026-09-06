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

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"

int main(void)
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    PRINTF("Member: <your name here>\r\n");
    PRINTF("Board ID: %08X\r\n", SIM->UIDL);

    while (1)
    {
    }
}
