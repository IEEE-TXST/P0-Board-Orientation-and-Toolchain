/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Blinks the onboard LED by toggling one GPIO pin forever, with a busy-wait
   delay in between. No interrupts, no timers, just the simplest possible way
   to move a pin. This is the standard first program for a new board: it
   proves the whole toolchain works (write, compile, flash, run) using the
   one piece of hardware you can verify just by looking at it. See
   P0_3_Setup_and_Walkthrough.md Section 8, and Section 8.2 for an exercise
   that builds on this file. */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"

#include "clock_config.h"
#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Picks which physical LED this program controls. BOARD_LED_RED_GPIO /
   _PIN come from board.h, which already knows which port and pin the red
   LED is wired to. Aliasing it to BOARD_LED_GPIO here, instead of writing
   BOARD_LED_RED_GPIO everywhere below, means Section 8.2's color exercise
   is just a two-line edit. */
#define BOARD_LED_GPIO BOARD_LED_BLUE_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_BLUE_GPIO_PIN

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Busy-waits so the LED stays on/off long enough to see (toggling faster
   than roughly 30-60 Hz just looks like a dim, steady glow to the eye).
   `volatile` matters: without it the compiler could legally decide this
   loop has no effect and delete it entirely. This wastes CPU on purpose,
   fine for a first demo, but P1's PIT-driven dashboard shows the real way
   to pace a program. */
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 800000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

/*!
 * @brief Main function
 */
/* Sets up the board and the LED pin once, then blinks forever: init pins,
   clocks, and debug console; init the LED pin; loop delaying and toggling.
   This init-once-then-loop-forever shape is the "superloop" pattern almost
   every program in this series follows, worth recognizing here before the
   busier P1-P4 main() functions use it too. */
int main(void)
{
    /* Define the init structure for the output LED pin*/
    /* kGPIO_DigitalOutput means this pin drives a voltage out, not reads
       one in; the trailing 0 sets its initial level to low so there's no
       undefined moment right as it's configured. */
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };

    /* Board pin, clock, debug console init */
    /* Three one-time setup calls every project in this series starts with:
       pins, then the clock tree, then the debug UART PRINTF() needs. The
       chip powers up in a generic default state, so nothing below works
       until these run first. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Print a note to terminal. */
    /* Confirms the debug console (and your terminal setup) works,
       independent of the LED. If you see this text but no blink, the bug
       is in the GPIO code below, not the toolchain. */
    PRINTF("\r\n GPIO Driver example\r\n");
    PRINTF("\r\n The LED is taking turns to shine.\r\n");

    /* Init output LED GPIO. */
    /* Applies led_config to the real pin: the struct above was just a
       description sitting in memory until this call writes it into the
       chip's actual hardware registers, the point where the pin truly
       becomes an output. */
    GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config);

    /* Runs forever: delay, then flip the LED pin's voltage, repeat.
       Embedded programs don't "return" from main() the way desktop
       programs do, there's no OS waiting to take back control, so the chip
       just keeps executing whatever is in flash until power is removed. */
    while (1)
    {
        delay();
        GPIO_TogglePinsOutput(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
    }
}
