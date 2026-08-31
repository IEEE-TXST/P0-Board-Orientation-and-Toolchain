# P0 Demo Code

Reference material only. Try building each step yourself from the Project Manual first;
open these only if you get stuck and can't figure out why.

- `01_led_blink_demo/` — the pre-built LED blink demo (Section 7 of the manual). Source,
  buildable armgcc project, and the already-flashed `.srec`.
- `02_uart_hello_world_reference/` — a reference solution for the UART hello world task
  (Section 8). Board-support files (`board.c/h`, `pin_mux.c/h`, `clock_config.c/h`) are
  copied straight from the SDK's `hello_world` example for `frdmkl26z`; `main.c` is the
  reference implementation. Replace the name placeholder and write your own version
  before peeking at this one.
