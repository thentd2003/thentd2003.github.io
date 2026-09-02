/*
  EDIT THIS FILE to update your portfolio. index.html and style.css never
  need to change — just edit the values below, save, and push.

  - Anything wrapped in [ ] is a placeholder — replace it with real info.
  - To add a photo to a project, set "image" to a path like "thumbnails/aurasleep.jpg"
    and fill in "imageAlt". Leave image as null to keep the dashed placeholder box.
  - To add another project, copy one of the objects in the "projects" array below
    (including the outer { } and comma) and edit its fields. Order in this file
    is the order it appears on the page.
*/

window.SITE_DATA = {

  name: "David Nguyen",
  headlineHighlight: "the circuits",
  tagline: "Power electronics and embedded systems engineer — resonant converters, control loops, and the boards that make hardware products actually run. Currently building the hardware for AuraSleep and searching for my final co-op.",
  meta: ["Simon Fraser University", "Burnaby, BC, Canada", "Open to Co-op — Spring – Summer 2027"],

  resumeLink: "Dave_resume_Engineering.pdf",
  email: "dtn7@sfu.ca",
  github: "https://github.com/thentd2003",
  linkedin: "https://linkedin.com/in/david-nguyen-53377a268",

  projects: [
    {
      designator: "U1",
      section: "embedded",
      status: "In Progress",
      statusClass: "is-progress",
      title: "AuraSleep — Smart Sleep Mask",
      blurb: "Founder & hardware lead on a sleep-tech wearable — currently taking the hardware from concept to a first working milestone.",
      tags: ["PCB Design", "Embedded Systems", "Product Dev"],
      image: "thumbnails/aurasleep.jpg",
      imageAlt: "AuraSleep sleep mask prototype",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/aurasleep",
      linkLabel: "Full Report",
      details: {
        "Problem": "Founded AuraSleep to build a wearable sleep mask that combines comfort with onboard sensing/intervention hardware — moving from an engineering background into leading full product development.",
        "My role": "Founder and hardware lead — driving the hardware roadmap from concept toward a first working prototype, alongside product and go-to-market decisions.",
        "Status": "[Add current milestone — e.g. \"first working prototype achieved MM/YYYY\" — plus any specs, sensor list, or power budget once finalized.]"
      }
    },
    {
      designator: "U2",
      section: "power",
      status: "Complete",
      statusClass: "",
      title: "Half-Bridge Resonant Induction Heater",
      blurb: "An LCLR resonant induction heater built for a hardware startup, now mid-redesign toward closed-loop PLL frequency control.",
      tags: ["Half-Bridge", "LCLR Resonant Tank", "PLL Control"],
      image: "thumbnails/induction_heater.jpg",
      imageAlt: "Half-bridge induction heater build",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/induction-heater",
      linkLabel: "Full Report",
      details: {
        "Problem": "Built for a hardware startup that needed a working induction heating stage — a half-bridge driving an LCLR resonant tank to deliver controlled heating power.",
        "Current work": "Redesigning the control approach toward closed-loop PLL tracking, so the drive frequency follows the tank's resonant point automatically instead of running open-loop.",
        "Outcome": "[Add power delivered, switching frequency range, and what the PLL redesign is expected to improve once complete.]"
      }
    },
    {
      designator: "U3",
      section: "power",
      status: "Complete",
      statusClass: "",
      title: "High-Voltage Flyback Converter",
      blurb: "My first fully independent power electronics design — a flyback converter and Cockcroft-Walton multiplier built during a co-op at Analytic Systems to generate 1000V test surges for validating a high-voltage inverter's protection board.",
      tags: ["Flyback Converter", "Cockcroft-Walton Multiplier", "SMPS Design"],
      image: "thumbnails/flyback.png",
      imageAlt: "HV flyback converter board",
      mediaLabel: "+ Add photo / schematic",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/flyback-converter",
      linkLabel: "Full Report",
      details: {
        "Problem": "Analytic Systems needed to validate a high-voltage surge disconnect board — protecting a high-voltage inverter from surges up to 1000V (e.g. from lightning) — by generating controlled test surges from a 24V, 2A supply. A flyback converter was chosen for its natural isolation and wide output voltage range.",
        "Approach": "Built a 555-timer oscillator driving a flyback transformer (7.7:1 step-up) through a MOSFET switch, feeding a 7-stage Cockcroft-Walton multiplier to reach up to 1000V. Iterated through two major redesigns after real failures: switched to a 650V MOSFET and added an RCD snubber after transformer kickback destroyed the original 200V device, then added dual coarse/fine potentiometers, a gate resistor, and active cooling to fix control and thermal issues. Packaged in a custom SolidWorks-designed, 3D-printed enclosure.",
        "Outcome": "Delivered an adjustable 300–1000V surge source in a safety-labeled enclosure, used to successfully test the disconnect board. Also identified the design's key limitation — nonlinear output once loaded, from the multiplier's sensitivity to waveform shape — and recommended a dedicated flyback controller IC as the fix for a future revision."
      }
    },
    {
      designator: "U4",
      section: "power",
      status: "Complete",
      statusClass: "",
      title: "CMOS Bandgap Reference Voltage Circuit",
      blurb: "A CMOS bandgap voltage reference designed and simulated in OrCAD X — a PTAT/CTAT current-mirror topology with startup circuitry, tuned to under 100 ppm/°C temperature drift.",
      tags: ["Analog IC Design", "OrCAD X", "Team of 3"],
      image: "thumbnails/bandgap.png",
      imageAlt: "CMOS bandgap reference circuit schematic",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/bandgap-reference",
      linkLabel: "Full Report",
      details: {
        "Problem": "Design and simulate a CMOS bandgap reference (BGR) circuit for an analog IC design course — a stable ~1.2V output largely independent of temperature and supply variation, built from a provided 5μm CMOS process.",
        "Approach": "Chose a current-mirror-based topology over a Brokaw cell for simpler CMOS implementation, combining PTAT and CTAT currents from diode-connected BJTs through a cascoded current mirror, with dedicated startup circuitry to guarantee correct power-on behavior every time.",
        "My contribution": "Simulations and report — building and iterating the OrCAD X simulations and writing up the results.",
        "Outcome": "Measured a 99.6 ppm/°C temperature coefficient across 0–100°C (target was ≤100 ppm/°C), 5mW power consumption, and — after adding 10μF/0.1μF output decoupling — near-complete suppression of ripple from a 1VAC line-noise injection test."
      }
    },
    {
      designator: "U5",
      section: "embedded",
      status: "In-Progress",
      statusClass: "",
      title: "Automated Motorized Soccer Tripod",
      blurb: "A motorized tripod that pans automatically to keep a soccer ball centered in frame, built for hands-free match filming — mechanical design, firmware, and BLE control all self-built, currently in hardware calibration.",
      tags: ["Motor Control", "Embedded Systems", "Automation"],
      image: "thumbnails/soccer_tripod.jpg",
      imageAlt: "Automated motorized soccer tripod",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/soccer-tripod",
      linkLabel: "Full Report",
      details: {
        "Problem": "Wanted a reliable way to film full soccer matches with my team without needing someone dedicated to holding a phone — for reviewing and improving play, and just as often, to see ourselves play.",
        "Approach": "Designed the full pan drive in SolidWorks — reverse-engineered a bought tripod's mounting socket by hand, then built the platform, gears, and bearings around it. Mid-build, the SG90 servo the housing was designed for turned out to max out at 180°, so I swapped to a continuous-rotation SG90 and wrote a software encoder (dead-reckoned position tracking corrected against a limit switch) to recover positional control. Own the BLE protocol between phone and ESP32, plus the tracking algorithm converting the phone's ball-offset detection into motor commands.",
        "Outcome": "Hardware built and integrated; currently calibrating motion constants (speed, timing, homing) against the physical rig."
      }
    },
    {
      designator: "U6",
      section: "embedded",
      status: "Complete",
      statusClass: "",
      title: "RISC-V Pipeline Simulator",
      blurb: "A cycle-accurate RISC-V pipeline simulator in C, built with a 4-person team across four milestones for a computer architecture course.",
      tags: ["C", "Cache Hierarchy", "Branch Prediction"],
      image: "thumbnails/riscv.png",
      imageAlt: "5-stage RISC-V pipeline diagram",
      mediaLabel: "+ Add screenshot",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/riscv-simulator",
      linkLabel: "Full Report",
      details: {
        "Milestones": "Basic 5-stage pipeline → hazard detection & forwarding → cache integration → two-level cache + branch prediction as a final milestone.",
        "My contribution": "Control and hazard/forwarding logic, the cache subsystem, and the branch predictor's test cases and final report.",
        "Outcome": "All milestones passed. Caching gave an 11× speedup overall, and the 2-bit branch predictor reached ~97% accuracy."
      }
    },
    {
      designator: "U7",
      section: "embedded",
      status: "Complete",
      statusClass: "",
      title: "FPGA Digital Clock",
      blurb: "A VHDL digital clock on a Cyclone V FPGA (Terasic DE10-Standard) — BCD timekeeping, button-driven time/alarm setting, and a buzzer alarm, verified with dedicated testbenches.",
      tags: ["VHDL", "FPGA", "Digital Logic"],
      image: "thumbnails/digital_clock.jpg",
      imageAlt: "FPGA digital clock on DE10-Standard dev board",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/fpga-digital-clock",
      linkLabel: "Full Report",
      details: {
        "Problem": "Built as a digital logic project: a real-time clock running entirely on FPGA fabric, with time and alarm both set through onboard buttons — no external microcontroller.",
        "Approach": "Structured as small VHDL modules — a prescaler dividing the 50MHz board clock down for timekeeping, a BCD time counter, a button-driven mode FSM to move between display/set-time/set-alarm states, an alarm-compare block, and a buzzer driver — instantiated together in a top-level entity and checked with dedicated testbenches for the mode FSM and the full clock datapath.",
        "My contribution": "[Specify which modules you personally wrote or verified within the team.]",
        "Outcome": "Synthesized cleanly on the Cyclone V target — 186 of 41,910 ALMs (< 1%) and 153 registers used. [Add confirmation of hardware demo/testing and any timing notes.]"
      }
    }
  ],

  about: {
    paragraphs: [
      "I come from a hardware engineering background and have been moving toward leading full products, not just circuits — founding AuraSleep pushed me to own everything from PCB design to the product roadmap.",
      "I'm most energized by hands-on technical work: power electronics, embedded systems, and the kind of debugging that happens with an oscilloscope probe in hand. I'm currently looking for my final co-op placement, ideally somewhere I can keep building hands-on hardware at a larger scale."
    ],
    facts: [
      ["Focus", "Power Electronics & Embedded Systems"],
      ["Currently", "Founder, AuraSleep"],
      ["Looking for", "Final Co-op Placement"],
      ["Studying in", "Canada"]
    ]
  }

};
