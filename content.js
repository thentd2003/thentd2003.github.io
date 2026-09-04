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
  headlineHighlight: "circuits",
  tagline: "Power electronics and embedded systems engineer — resonant converters, control loops, and the boards that make hardware products actually run. Currently searching for my final co-op.",
  meta: ["Simon Fraser University", "Burnaby, BC, Canada", "Open to Co-op — Spring – Summer 2027"],

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
      blurb: "A 500W-class, 12V half-bridge LCLR resonant induction heater — recovered from an unstable first driver design to a validated V1 (60°C in 60s), now rebuilt around a closed-loop PLL for real-time frequency tracking through the workpiece's Curie point.",
      tags: ["Half-Bridge", "LCLR Resonant Tank", "PLL Control"],
      image: "thumbnails/induction_heater.jpg",
      imageAlt: "Half-bridge induction heater build",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/induction-heater",
      linkLabel: "Full Report",
      details: {
        "Problem": "Joined a friend's hardware startup as lead engineer, tasked with designing and building the company's induction heater from scratch — meeting his product requirements and integrating it with his MCU/control board.",
        "Approach": "A first working prototype came together in about a month using a basic Mazzilli ZVS driver, but it was quickly clear the circuit had no real control over its own switching — too unpredictable and unstable for a commercial product. I spent the next several months teaching myself induction heating theory from the ground up and rebuilt the circuit myself, hand-selecting the operating frequency and sizing the tank capacitors and coil inductance. Along the way I burned through MOSFETs, killed gate driver ICs, and worked through repeated overcurrent failures before landing on a stable design.",
        "Outcome": "Reached a functional V1 prototype in April 2026, meeting the target of heating a steel cup of water to 60°C in 60 seconds. The startup lost its funding and shut down before I could finish the integration work, so I've since kept upgrading the design on my own — replacing the potentiometer-set operating frequency, which can't track the tank's resonance drifting as the steel workpiece crosses its Curie point mid-heat, with a PLL feedback loop that locks the drive frequency to the optimal point automatically. PCB design for that revision is in progress now."
      }
    },
    {
      designator: "U2",
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
      designator: "U3",
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
      designator: "U4",
      section: "embedded",
      status: "On Hold",
      statusClass: "",
      title: "AuraSleep — Smart Sleep Mask",
      blurb: "A hardware startup I co-founded with two partners to build a sleep-tech wearable — validated through customer research and mentor pitches, developed through to finished schematics.",
      tags: ["PCB Design", "Embedded Systems", "Product Dev"],
      image: "thumbnails/aurasleep.jpg",
      imageAlt: "AuraSleep sleep mask prototype",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/aurasleep",
      linkLabel: "Full Report",
      details: {
        "Problem": "Co-founded AuraSleep with two partners to build a wearable sleep mask combining comfort with onboard sensing hardware. Before building anything, we ran market research, surveys, and customer interviews to confirm the problem and the market gap were real.",
        "My role": "Co-founder and hardware lead — driving PCB and electronics design, while also working the business side: pitching to mentors repeatedly, taking their feedback (not always easy to hear), and redesigning the product each time we learned something new.",
        "Status": "Finished schematics and the full technology research, with PCB design and mask integration as the planned next step."
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
      section: "digital",
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
      section: "digital",
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
      "Curiosity is where this all started. As a kid I'd take things apart just to understand them — chasing YouTube tutorials and DIY shows, then building my own breadboard-messy projects once I got my hands on an Arduino in high school, teaching myself to code because I wanted my ideas to actually do something.",
      "A coop placement is where that curiosity turned into a calling. Seeing a product come together — PCB design, embedded systems, real engineering decisions with real consequences — hit different than anything in a classroom, and it was enough to make me switch my major to ENSC's electronics concentration without a second thought. I'd already taught myself CAD and bought a 3D printer on faith before that term even started, and when the coop finally gave me a real problem to solve — designing a test jig from scratch — I threw everything I'd taught myself at it. That's who I am: give me a goal I don't know how to reach yet, and I will find a way to learn into it.",
      "Today that drive shows up as real, tangible skill. I'm strongest in circuit and PCB design — carrying something from a schematic to a working, tested board — and I care enough about it to keep a personal electronics lab bench at home. I can hold my own in firmware and code, and in SolidWorks when a prototype or test jig needs to exist in the physical world. What I'm really proud of is the whole loop: I can take an idea and carry it, myself, from a rough sketch to something real in my hands.",
      "Wanting to build something entirely my own is what led me to co-found AuraSleep with two partners. We didn't just chase the engineering — we ran market research, surveys, and customer interviews to make sure the problem we were solving was real, and pitched to mentors more times than I can count. Some of that feedback stung. We kept going anyway, redesigning and reframing the product every time we learned something new, until we had working schematics and the technology fully researched. It didn't end the way I'd hoped, but running my own startup taught me more about resilience, and about myself, than almost anything else I've done — and it changed how I think about building things for good.",
      "Outside of engineering, I chase the same restlessness in other ways — soccer, hiking, badminton, tennis, and traveling to new countries just to see how other people live and build their lives. I'm currently looking for my final co-op placement, and I'm looking for the same thing I've always chased: a problem worth throwing myself at."
    ]
  }

};
