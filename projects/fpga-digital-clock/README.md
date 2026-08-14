# FPGA Digital Clock

VHDL digital clock built for a Cyclone V FPGA dev board (device 5CSXFC6D6F31C6).

## Modules
- `Prescale_Group10` — divides the 50MHz board clock down for timekeeping
- `TimeCounter_Group10` — BCD hours/minutes/seconds counter, loadable for time-set
- `BCDCount2_Group10` / `Count4_Group10` — supporting BCD counters
- `DisplayDriver_Group10` — seven-segment display driver (HEX0-HEX5)
- `ModeFSM_Group10` — button-driven mode FSM (display / set-time / set-alarm)
- `TimeSetControl_Group10` — digit-by-digit time entry logic
- `AlarmCompare_Group10` / `Buzzer_Group10` — alarm comparison and buzzer output
- `LCD_Controller` / `LCD_Initializer` / `LCDAlarmTop` — LCD-based alarm display variant
- `DigitalClockTop_Group10` — top-level entity tying everything together
- `tb_*` — testbenches for the mode FSM and full clock datapath

## Synthesis results (Quartus Prime 20.1.1, Cyclone V)
- Logic utilization: 186 / 41,910 ALMs (< 1%)
- Registers: 153
- Pins used: 61 / 499 (12%)

Drop any additional docs, photos, or demo footage links here.
