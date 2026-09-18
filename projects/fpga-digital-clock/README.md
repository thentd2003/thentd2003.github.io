# FPGA Digital Clock

A real-time clock built entirely in VHDL for an Intel/Altera Cyclone V FPGA (Terasic DE10-Standard, device `5CSXFC6D6F31C6`). No microcontroller involved: timekeeping, button-driven time/alarm setting, and the alarm buzzer are all implemented as dedicated hardware logic.

## What it does

- Displays time as `HH:MM:SS` (24-hour) across six seven-segment displays (`HEX0`–`HEX5`)
- `SW(0)` enables menu mode; `KEY(1)` cycles **Display → Set Time → Set Alarm → Display**
- While in Set Time / Set Alarm: `KEY(2)` moves to the next digit (cycles through all 6), `KEY(0)` increments the selected digit, `KEY(3)` commits the edited value into the running clock or the stored alarm
- The digit currently being edited blinks at 1 Hz
- When the running time matches the stored alarm, the display switches to `AAAAAA` and the buzzer output goes high; `SW(2)` dismisses the alarm

## Architecture

Everything is wired together in the top-level entity, `DigitalClockTop_Group10`:

| Module | Role |
|---|---|
| `PreScale_Group10` | Divides the 50 MHz board clock down to 1 Hz, which is what the clock actually counts on |
| `ModeFSM_Group10` | One-hot FSM for the three modes. Debounces all four buttons with rising-edge detection, tracks which digit is selected for editing, and generates one-cycle commit pulses |
| `TimeSetControl_Group10` | Holds the "live edit" buffer while a user is dialing in a new time or alarm value, each digit wraps correctly (0–9 for ones-digits, 0–5 for the tens of seconds/minutes, and hours handled separately) |
| `TimeCounter_Group10` | The actual running clock: a BCD seconds/minutes/hours counter with correct 24-hour wraparound (`23:59:59 → 00:00:00`), plus a synchronous load path for committing an edited time |
| `AlarmCompare_Group10` | Combinational 6-digit comparator between the live time and the stored alarm. Ignores the alarm entirely while it's still at its default `99:99:99` sentinel, so an unset alarm never fires |
| `DisplayDriver_Group10` | BCD-to-seven-segment decoding, the blink override on whichever digit is being edited, and the `AAAAAA` display state when the alarm is triggered |

**Clock-domain crossing:** the mode FSM generates its commit pulses on the 50 MHz clock, but the time counter runs on the divided-down 1 Hz clock. A single-cycle pulse on the 50 MHz domain is far too narrow to reliably land on a 1 Hz clock edge, so the top level double-registers the request (`ack1`/`ack2` for time, `a1`/`a2` for alarm) across that boundary before the counter is allowed to actually load new data.

## What's in the folder but *not* part of the final design

A few files here are earlier iterations or standalone hardware bring-up tests, kept for reference rather than wired into `DigitalClockTop_Group10`:

- **`Count4_Group10` / `BCDCount2_Group10`**: an earlier, more generic BCD-counter approach (a ripple-carry incrementer with a configurable rollover value). It was superseded by the direct if/elsif cascading increment logic inside `TimeCounter_Group10`, which was simpler to get right for the specific 24-hour/60-minute/60-second rollover rules.
- **`Buzzer_Group10`**: a standalone tone-generating buzzer driver (toggles at 1 kHz). The final top-level design drives `BUZZER` directly off the alarm flag instead of routing through this module.
- **`BuzzerTest_Top`** and **`LCDAlarmTop` / `LCD_Controller` / `LCD_Initializer`**: separate top-level entities used to bring up and test the buzzer and an LCD blink indicator on real hardware in isolation, before integrating equivalent behavior into the main HEX-display design.

## Verification

- `tb_ModeFSM_Group10`: exercises the mode FSM's state transitions and button debouncing on its own
- `tb_DigitalClockTop_Group10` drives the full top-level entity: power-up, enable mode select, confirm into Display mode, then enable the clock display and let it free-run
- `tb_AlarmDisplayTop`, `Test_DisplayAlarmTop`: additional testbenches around the alarm/display path

## Synthesis results (Quartus Prime 20.1.1, Cyclone V)

- Logic utilization: 186 / 41,910 ALMs (< 1%)
- Registers: 153
- Pins used: 61 / 499 (12%)
