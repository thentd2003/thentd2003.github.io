# SoccerCam

A motorized phone-tripod head that tracks a soccer ball in real time. The phone runs on-device ball detection and sends movement commands over Bluetooth to a microcontroller, which drives pan/tilt motors to keep the ball framed — no separate camera, no cloud processing, no wireless-tethered motor unit.

This repo covers the **hardware, mechanical design, and firmware** side of the project.

## Status

🔧 Hardware built and integrated — currently in calibration (tuning empirical motion constants against the physical build).

## How it works

1. The phone detects the ball on-device (Apple Vision/CoreML) and computes a normalized offset from center.
2. That offset is run through a proportional controller with an expo response curve and deadzone, producing a signed speed command (e.g. `T:-42`).
3. The command is sent over BLE to the ESP32.
4. The ESP32 scales the command into motor output and drives the pan (servo) / tilt (stepper) motors, enforcing soft and hard travel limits.
5. Position is tracked via dead reckoning, corrected against a limit switch at the home position (the continuous-rotation servo used for pan has no absolute position feedback, so this correction is what makes homing possible).
6. A tracking watchdog stops the motor if BLE updates go stale, so the rig doesn't run away if the phone loses tracking.

## Hardware

| Component | Part |
|---|---|
| Microcontroller | ESP32 |
| Pan motor | SG90 continuous-rotation servo |
| Tilt motor | 28BYJ-48 stepper + ULN2003AN driver (placeholder) |
| Homing | SPDT micro limit switch |
| Bearing | 608ZZ (8×22×7mm) |
| Gearing | Custom 3D-printed bevel gears — module 1.5, 45T fixed / 30T pinion, 56.25mm center distance, 1.5:1 ratio |
| Frame | 3D-printed platform, mounted on an off-the-shelf tripod (socket reverse-engineered by hand) |

**Note:** the SG90 *positional* servo can't rotate past 180°, but the housing and mounts were already designed around its footprint. Rather than redesign for a different motor, this project swaps in the continuous-rotation SG90 variant and implements a software encoder — dead-reckoned position tracking, corrected against the limit switch — to recover positional behavior.

**Power:** the SG90 needs an external 5V supply; powering it from the ESP32's onboard 5V pin causes brownouts under load.

## Firmware

Single-file non-blocking state machine (`SoccerCam.ino`), built on:

- **NimBLE-Arduino** (h2zero) — BLE server, receives tracking commands from the phone
- **ESP32Servo** (Kevin Harrington / John K. Bennett) — pan servo control
- **AccelStepper** — tilt stepper control (placeholder)

Key design points:

- `millis()`-based non-blocking timing throughout — no `delay()` calls, so the rig stays responsive and interruptible
- Unified `enforceLimits()` function applies both soft and hard travel limits consistently
- Dead-reckoned position estimate, corrected against limit-switch ground truth on homing
- Proportional tracking controller with deadzone on the phone side; the ESP32 only handles mechanical scaling (`map()`) — it doesn't own any tracking logic

### Constants requiring hardware calibration

These are empirically tuned against the physical build, not computed:

- `HOME_DIRECTION`
- `DEG_PER_SEC_AT_MAX_SPEED`
- `TRACKING_TIMEOUT_MS`
- Expo curve exponent (`1.6`, Swift side)
- `PAN_MOVE_TO_CENTER_DURATION_MS`

## Mechanical design

Designed in SolidWorks (Toolbox for gear configuration). Bevel gears were chosen over a worm-gear approach because FDM-printed worm gears are unreliable at these tolerances, and the layout — servo not co-located with the rotating platform, platform on its own dedicated bearing — calls for right-angle torque transmission with both gears supported close to their respective bearings to avoid mesh flex under load.

Bearing seat bores follow: `bore = nominal OD + (0.53–0.6 × nozzle diameter)`. A relief cut on the gear hub's underside ensures the retaining nut contacts only the bearing's outer race, preventing the race from locking up.

## Firmware upload note

Manual BOOT-button upload is required — the EN switch's capacitance interferes with auto-reset on this board.

## Roadmap

- Finish hardware calibration of all empirical constants
- Android support via a proprietary BLE app (avoids dependence on Apple's DockKit)
- Improve tracking robustness for multi-ball scenarios (e.g. set pieces)

## Credits

Mechanical design, embedded firmware, BLE protocol, and tracking/vectoring algorithm by me. iOS app and on-device AI ball detection by a collaborator.
