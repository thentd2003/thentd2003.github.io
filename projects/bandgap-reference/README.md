# CMOS Bandgap Reference Voltage Circuit

ENSC 325 (Analog IC Design) final project — a CMOS bandgap reference (BGR)
circuit designed and simulated in OrCAD X / PSpice.

## Design
- Current-mirror-based PTAT/CTAT topology (chosen over a Brokaw cell for
  simpler CMOS implementation)
- Cascoded current mirror (M1-M4) biasing diode-connected PNP BJTs
- Dedicated startup circuit (M5-M8) to guarantee correct power-on behavior
- Built on the course-provided 5μm BNR CMOS process

## Results
- Temperature coefficient: 99.6 ppm/°C across 0-100°C (target: ≤100 ppm/°C)
- Power consumption: 5 mW
- Line noise rejection verified with a 1VAC injection test; ripple nearly
  fully suppressed after adding 10μF/0.1μF output decoupling

## Files
- `bandgap-reference.dsn` / `.opj` — OrCAD schematic and project files
- `q2n3906.lib` / `5um_bnr_cmos.lib` — SPICE models used in simulation
- `ENSC_325_Bandgap_Reference_Report.pdf` — full write-up with derivations,
  schematics, and test results

Drop any additional docs or photos here.
