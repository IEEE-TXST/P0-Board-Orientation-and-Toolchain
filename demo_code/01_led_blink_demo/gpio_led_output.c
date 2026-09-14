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

/*
 * WHAT: Blinks the FRDM-KL26Z's onboard red LED by toggling one GPIO pin
 * forever, with a busy-wait delay in between toggles.
 *
 * HOW: Configure one pin as a digital output, then loop forever: wait a bit,
 * flip the pin's output voltage, repeat. No interrupts, no timers, just a
 * plain "spin and flip" loop, the simplest possible way to move a pin.
 *
 * WHY: This is the very first program most people write for a new board,
 * embedded's equivalent of "hello world". It proves the whole toolchain
 * works end to end (write code, compile, flash, run) using the one piece of
 * hardware you can verify just by looking at it: an LED either blinks or it
 * doesn't. See P0_3_Setup_and_Walkthrough.md Section 8 for the full
 * step-by-step and Section 8.2 for an exercise that builds on this file.
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"

#include "clock_config.h"
#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*
 * WHAT: Picks which physical LED this program controls.
 * HOW: BOARD_LED_RED_GPIO / BOARD_LED_RED_GPIO_PIN come from board.h, which
 * already knows (from the schematic) which GPIO port and pin number the red
 * LED is wired to on this exact board.
 * WHY: Naming it BOARD_LED_GPIO here, instead of writing BOARD_LED_RED_GPIO
 * everywhere below, means Section 8.2's color-change exercise is just a
 * two-line edit: swap these two defines to the green or blue equivalents and
 * every line below still works unchanged.
 */
#define BOARD_LED_GPIO BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN

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
/*
 * WHAT: Burns CPU time doing nothing useful, so the LED stays on/off long
 * enough for a human eye to actually see it.
 * HOW: Counts up to 800,000, executing one NOP ("no operation", do nothing
 * for one cycle) each time through the loop.
 * WHY: Without this, the CPU would toggle the pin millions of times per
 * second and the LED would just look dimly, constantly on, the human eye
 * can't see anything faster than roughly 30-60 toggles per second. This is
 * called a "busy-wait" delay: it wastes CPU cycles on purpose instead of
 * using a hardware timer, which is fine for a first demo but not how real
 * firmware paces itself (P1's PIT-driven dashboard shows the proper way).
 * `volatile` on the loop counter matters here: it tells the compiler not to
 * optimize this "pointless" loop away, since without it the compiler could
 * legally decide the loop has no observable effect and delete it entirely.
 */
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
/*
 * WHAT: Sets up the board and the LED pin once, then blinks forever.
 * HOW: Three stages: (1) one-time board bring-up (pins, clocks, debug
 * console), (2) one-time GPIO pin setup, (3) an infinite loop that delays
 * then toggles the pin.
 * WHY: This three-stage shape, init once then loop forever, is the pattern
 * almost every embedded program in this series follows (it's literally
 * called the "superloop" in later projects). Learning to recognize it here,
 * in the simplest possible example, makes the busier P1-P4 main() functions
 * much easier to read later.
 */
int main(void)
{
    /* Define the init structure for the output LED pin*/
    /*
     * WHAT: Describes how the LED pin should behave before it's turned on.
     * HOW: kGPIO_DigitalOutput means "this pin drives a voltage out", not
     * "this pin reads a voltage in". The trailing 0 sets the pin's initial
     * output level to logic low the moment it's configured.
     * WHY: A GPIO pin can be an input or an output; the chip needs to be
     * told which, since the hardware behaves completely differently in each
     * mode. Setting an explicit initial value (0) avoids a brief moment
     * where the pin's state is undefined right as it's configured.
     */
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };

    /* Board pin, clock, debug console init */
    /*
     * WHAT: Three one-time setup calls every project in this series starts
     * with.
     * HOW: BOARD_InitPins() applies the pin-mux table generated for this
     * board (which peripheral "owns" each physical pin); BOARD_BootClockRUN()
     * configures the chip's clock tree to the run-mode frequency this project
     * was built for; BOARD_InitDebugConsole() sets up the UART used by
     * PRINTF() below.
     * WHY: The chip powers up with pins and clocks in a generic default
     * state, not yet wired for what this program needs. Nothing below this
     * point works correctly until these three run first, which is why they
     * always come before any peripheral-specific code.
     */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Print a note to terminal. */
    /*
     * WHAT: Sends two lines of text out over the debug UART.
     * HOW: PRINTF works like standard C printf, but the bytes go out the
     * serial port instead of a screen; a terminal program on your laptop
     * (Section 5.3) displays them.
     * WHY: Confirms the debug console (and your terminal setup) is working,
     * independent of whether the LED blinks. If you see this text but no
     * blink, the problem is in the GPIO code below, not the toolchain.
     */
    PRINTF("\r\n GPIO Driver example\r\n");
    PRINTF("\r\n The LED is taking turns to shine.\r\n");

    /* Init output LED GPIO. */
    /*
     * WHAT: Applies the led_config settings above to the actual LED pin.
     * HOW: Takes the port/pin identified by BOARD_LED_GPIO /
     * BOARD_LED_GPIO_PIN and configures that specific pin's direction
     * register as an output, using led_config.
     * WHY: Defining a config struct and initializing a pin are two separate
     * steps on purpose: the struct is just a description sitting in memory
     * until this call actually writes it into the chip's hardware
     * registers, which is the point where the pin truly becomes an output.
     */
    GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config);

    /*
     * WHAT: Runs forever, alternating a delay and a pin toggle.
     * HOW: while (1) never exits; each pass waits (delay()), then flips the
     * LED pin's output voltage (GPIO_TogglePinsOutput).
     * WHY: This is the only code that ever runs after setup finishes.
     * Embedded programs normally never "return" from main() the way a
     * desktop program does, since there's no operating system waiting to
     * take back control. Per Section 1 of the manual, the chip just keeps
     * executing whatever is in flash, forever, until power is removed.
     */
    while (1)
    {
        delay();
        GPIO_TogglePinsOutput(BOARD_LED_GPIO, 1u << BOARD_LED_GPIO_PIN);
    }
}
