/*
  EDIT THIS FILE to update your portfolio. index.html and style.css never
  need to change. Just edit the values below, save, and push.

  - Anything wrapped in [ ] is a placeholder: replace it with real info.
  - To add a photo to a project, set "image" to a path like "thumbnails/aurasleep.jpg"
    and fill in "imageAlt". Leave image as null to keep the dashed placeholder box.
  - To add a company logo to a Work Experience entry, drop the image in the "logos/"
    folder and set "logo" to a path like "logos/tech-maple-works.png". Leave logo as
    null to keep the copper-colored initials badge instead (set "initials" to the
    2-letter code you want shown).
  - To add another project, copy one of the objects in the "projects" array below
    (including the outer { } and comma) and edit its fields. Order in this file
    is the order it appears on the page.
*/

window.SITE_DATA = {

  name: "David Nguyen",
  headline: 'David Nguyen, <em>power electronics</em> and embedded systems engineer.',
  tagline: "I design resonant power converters, flyback and multiplier topologies, and closed-loop embedded control systems, carrying each one from circuit theory and topology selection through PCB layout to validated, tested hardware.",
  meta: ["Simon Fraser University", "Burnaby, BC, Canada", "Open to Co-op: Spring – Summer 2027"],

  resumeLink: "David_Nguyen_Resume.pdf",
  email: "dtn7@sfu.ca",
  github: "https://github.com/thentd2003",
  linkedin: "https://linkedin.com/in/david-nguyen-53377a268",

  projects: [
    {
      designator: "U1",
      section: "power",
      status: "In Progress",
      statusClass: "is-progress",
      title: "Half-Bridge Resonant Induction Heater",
      blurb: "A 500W-class, 12V half-bridge LCLR resonant induction heater, recovered from an unstable first driver design to a validated V1 (60°C in 60s), now rebuilt around a closed-loop PLL for real-time frequency tracking through the workpiece's Curie point.",
      tags: ["Half-Bridge", "LCLR Resonant Tank", "PLL Control"],
      image: "thumbnails/induction_heater.jpg",
      imageAlt: "Half-bridge induction heater build",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/induction-heater",
      linkLabel: "Full Report",
      details: {
        "Problem": "Joined a friend's hardware startup as lead engineer, tasked with designing and building the company's induction heater from scratch, meeting his product requirements and integrating it with his MCU/control board.",
        "Approach": "A first working prototype came together in about a month using a basic Mazzilli ZVS driver, but it was quickly clear the circuit had no real control over its own switching, too unpredictable and unstable for a commercial product. I spent the next several months teaching myself induction heating theory from the ground up and rebuilt the circuit myself, hand-selecting the operating frequency and sizing the tank capacitors and coil inductance. Along the way I burned through MOSFETs, killed gate driver ICs, and worked through repeated overcurrent failures before landing on a stable design.",
        "Outcome": "Reached a functional V1 prototype in April 2026, meeting the target of heating a steel cup of water to 60°C in 60 seconds. The startup lost its funding and shut down before I could finish the integration work, so I've since kept upgrading the design on my own, replacing the potentiometer-set operating frequency, which can't track the tank's resonance drifting as the steel workpiece crosses its Curie point mid-heat, with a PLL feedback loop that locks the drive frequency to the optimal point automatically. PCB design for that revision is in progress now."
      }
    },
    {
      designator: "U2",
      section: "power",
      status: "Complete",
      statusClass: "",
      title: "High-Voltage Flyback Converter",
      blurb: "My first fully independent power electronics design: a flyback converter and Cockcroft-Walton multiplier built during a co-op at Analytic Systems to generate 1000V test surges for validating a high-voltage inverter's protection board.",
      tags: ["Flyback Converter", "Cockcroft-Walton Multiplier", "SMPS Design"],
      image: "thumbnails/flyback.png",
      imageAlt: "HV flyback converter board",
      mediaLabel: "+ Add photo / schematic",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/flyback-converter",
      linkLabel: "Full Report",
      details: {
        "Problem": "Analytic Systems needed to validate a high-voltage surge disconnect board, protecting a high-voltage inverter from surges up to 1000V (e.g. from lightning), by generating controlled test surges from a 24V, 2A supply. A flyback converter was chosen for its natural isolation and wide output voltage range.",
        "Approach": "Built a 555-timer oscillator driving a flyback transformer (7.7:1 step-up) through a MOSFET switch, feeding a 7-stage Cockcroft-Walton multiplier to reach up to 1000V. Iterated through two major redesigns after real failures: switched to a 650V MOSFET and added an RCD snubber after transformer kickback destroyed the original 200V device, then added dual coarse/fine potentiometers, a gate resistor, and active cooling to fix control and thermal issues. Packaged in a custom SolidWorks-designed, 3D-printed enclosure.",
        "Outcome": "Delivered an adjustable 300–1000V surge source in a safety-labeled enclosure, used to successfully test the disconnect board. Also identified the design's key limitation (nonlinear output once loaded, from the multiplier's sensitivity to waveform shape) and recommended a dedicated flyback controller IC as the fix for a future revision."
      }
    },
    {
      designator: "U3",
      section: "power",
      status: "Complete",
      statusClass: "",
      title: "CMOS Bandgap Reference Voltage Circuit",
      blurb: "A CMOS bandgap voltage reference designed and simulated in OrCAD X: a PTAT/CTAT current-mirror topology with startup circuitry, tuned to under 100 ppm/°C temperature drift.",
      tags: ["Analog IC Design", "OrCAD X", "Team of 3"],
      image: "thumbnails/bandgap.png",
      imageAlt: "CMOS bandgap reference circuit schematic",
      imageFit: "contain",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/bandgap-reference",
      linkLabel: "Full Report",
      details: {
        "Problem": "Design and simulate a CMOS bandgap reference (BGR) circuit for an analog IC design course: a stable ~1.2V output largely independent of temperature and supply variation, built from a provided 5μm CMOS process.",
        "Approach": "Chose a current-mirror-based topology over a Brokaw cell for simpler CMOS implementation, combining PTAT and CTAT currents from diode-connected BJTs through a cascoded current mirror, with dedicated startup circuitry to guarantee correct power-on behavior every time.",
        "My contribution": "Simulations and report: building and iterating the OrCAD X simulations and writing up the results.",
        "Outcome": "Measured a 99.6 ppm/°C temperature coefficient across 0–100°C (target was ≤100 ppm/°C), 5mW power consumption, and near-complete suppression of ripple from a 1VAC line-noise injection test after adding 10μF/0.1μF output decoupling."
      }
    },
    {
      designator: "U4",
      section: "embedded",
      status: "On Hold",
      statusClass: "",
      title: "AuraSleep: Smart Sleep Mask",
      blurb: "A hardware startup I co-founded with two partners to build a sleep-tech wearable, validated through customer research and mentor pitches, developed through to finished schematics.",
      tags: ["PCB Design", "Embedded Systems", "Product Dev"],
      image: "thumbnails/aurasleep.jpg",
      imageAlt: "AuraSleep sleep mask prototype",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/aurasleep",
      linkLabel: "Full Report",
      details: {
        "Problem": "Co-founded AuraSleep with two partners to build a wearable sleep mask combining comfort with onboard sensing hardware. Before building anything, we ran market research, surveys, and customer interviews to confirm the problem and the market gap were real.",
        "My role": "Co-founder and hardware lead, driving PCB and electronics design, while also working the business side: pitching to mentors repeatedly, taking their feedback (not always easy to hear), and redesigning the product each time we learned something new.",
        "Status": "Finished schematics and the full technology research, with PCB design and mask integration as the planned next step."
      }
    },
    {
      designator: "U5",
      section: "embedded",
      status: "In-Progress",
      statusClass: "",
      title: "Automated Motorized Soccer Tripod",
      blurb: "A motorized tripod that pans automatically to keep a soccer ball centered in frame, built for hands-free match filming; mechanical design, firmware, and BLE control all self-built, currently in hardware calibration.",
      tags: ["Motor Control", "Embedded Systems", "Automation"],
      image: "thumbnails/soccer_tripod.jpg",
      imageAlt: "Automated motorized soccer tripod",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/soccer-tripod",
      linkLabel: "Full Report",
      details: {
        "Problem": "Wanted a reliable way to film full soccer matches with my team without needing someone dedicated to holding a phone, for reviewing and improving play, and just as often, to see ourselves play.",
        "Approach": "Designed the full pan drive in SolidWorks: reverse-engineered a bought tripod's mounting socket by hand, then built the platform, gears, and bearings around it. Mid-build, the SG90 servo the housing was designed for turned out to max out at 180°, so I swapped to a continuous-rotation SG90 and wrote a software encoder (dead-reckoned position tracking corrected against a limit switch) to recover positional control. Own the BLE protocol between phone and ESP32, plus the tracking algorithm converting the phone's ball-offset detection into motor commands.",
        "Outcome": "Hardware built and integrated; currently calibrating motion constants (speed, timing, homing) against the physical rig."
      }
    },
    {
      designator: "U6",
      section: "digital",
      status: "Complete",
      statusClass: "",
      title: "RISC-V Pipeline Simulator",
      blurb: "A cycle-accurate RISC-V pipeline simulator in C, built with a 4-person team across four milestones for a computer architecture course.",
      tags: ["C", "Cache Hierarchy", "Branch Prediction"],
      image: "thumbnails/riscv.png",
      imageAlt: "5-stage RISC-V pipeline diagram",
      imageFit: "contain",
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
      section: "digital",
      status: "Complete",
      statusClass: "",
      title: "FPGA Digital Clock",
      blurb: "A VHDL digital clock on a Cyclone V FPGA (Terasic DE10-Standard): BCD timekeeping, button-driven time/alarm setting, and a buzzer alarm, verified with dedicated testbenches.",
      tags: ["VHDL", "FPGA", "Digital Logic"],
      image: "thumbnails/digital_clock.jpg",
      imageAlt: "FPGA digital clock on DE10-Standard dev board",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/fpga-digital-clock",
      linkLabel: "Full Report",
      details: {
       "Problem": "A real-time clock running entirely on FPGA fabric: time and alarm both set via onboard buttons, no microcontroller.",
"Approach": "Small VHDL modules (prescaler, BCD time counter, button-driven mode FSM, alarm comparator, buzzer driver) wired together in a top-level entity and verified with testbenches for the mode FSM and full clock datapath.",
"My contribution": "Designed and implemented the entire datapath and control logic solo: the BCD time counter with 24-hour rollover, the one-hot mode FSM with button debouncing, digit-by-digit time/alarm entry, the alarm comparator, the display driver, the 50MHz→1Hz clock-domain-crossing logic for commit pulses, and the testbenches.",
"Outcome": "Synthesized cleanly on the Cyclone V target, using 186/41,910 ALMs (under 1%) and 153 registers, and ran correctly on the DE10-Standard, with all button, display, and alarm behavior working as designed."
      }
    }
  ],

  experience: [
    {
      company: "Tech Maple Works",
      role: "Engineering Consultant, Power Electronics",
      dates: "Dec 2025 – Jul 2026",
      location: null,
      logo: "logos/tech-maple-works.png",
      initials: "TM",
      link: null,
      bullets: [
        "Designed and built a 500W-class half-bridge LCLR resonant induction heater from scratch as lead engineer",
        "Rebuilt the driver circuit after an unstable first design, hand-selecting switching frequency, tank capacitance, and coil inductance",
        "Reached a validated V1 prototype, heating a steel cup of water to 60°C in 60 seconds",
        "Now redesigning around a closed-loop PLL for real-time frequency tracking through the workpiece's Curie point"
      ]
    },
    {
      company: "Analytic Systems Engineering",
      role: "Engineering Co-op",
      dates: "Sept 2023 – Dec 2024",
      location: "Delta, BC",
      logo: "logos/analytic-systems.png",
      initials: "AS",
      link: "https://www.analyticsystems.com",
      bullets: [
        "Designed a high-voltage flyback converter and Cockcroft-Walton multiplier to generate 1000V test surges for validating a protection board",
        "Iterated through two major redesigns after real hardware failures, including a transformer kickback that destroyed the original MOSFET",
        "Delivered an adjustable 300–1000V surge source in a safety-labeled enclosure, used to test the disconnect board",
        "Supported EMC/EMI compliance testing on the team's power conversion products"
      ]
    },
    {
      company: "SFU Rocketry Team",
      role: "Power Team Member",
      dates: "Jul 2022 – Dec 2023",
      location: "Surrey, BC",
      logo: "logos/sfu-rocketry.png",
      initials: "SR",
      link: "https://www.sfurocketry.com/",
      bullets: [
        "Using Altium, LTspice for circuit design",
        "Built dynamic load testing system",
        "Researched and developed stable 5V and 3.3V converter",
        "Integrated converter into rocket's power system"
      ]
    },
    {
      company: "SFU Satellite & UBC Orbit",
      role: "Radio Communication Team Member",
      dates: "Jul 2022 – Jan 2023",
      location: "Burnaby, BC",
      logo: "logos/sfu-sat.png",
      initials: "SS",
      link: "https://www.ubcorbit.com/",
      bullets: [
        "Researching and developing Antenna Systems",
        "Learn about RF circuits and different types of antennas",
        "Get hands-on RF circuit and radio wave theory"
      ]
    }
  ],

  about: {
    paragraphs: [
      "I've been taking things apart since I was a kid, playing with loose motors and components, pulling old electronics apart just to see how they worked. The first real thing I built was a simple RC car off a breadboard in high school, and once it actually worked I was hooked for good. I knew right then engineering was going to be my career. What a coop placement settled a couple years later was which kind: I got handed a real problem to solve, a test jig from scratch, and switched my major to electronics without a second thought. These days I keep a personal electronics lab bench at home, where the projects are a lot less breadboard and a lot more likely to catch fire if I mess something up, like the resonant induction heater I rebuilt from scratch after burning through more MOSFETs than I'd like to admit. Still just as fun. I'm currently on the hunt for my final co-op, looking for the same thing I've always chased: a problem worth throwing myself at."
    ]
  }

};
