/*
  EDIT THIS FILE to update your portfolio. index.html and style.css never
  need to change — just edit the values below, save, and push.

  - Anything wrapped in [ ] is a placeholder — replace it with real info.
  - To add a photo to a project, set "image" to a path like "images/aurasleep-1.jpg"
    (put the file in an "images" folder next to this one) and fill in "imageAlt".
    Leave image as null to keep the dashed placeholder box.
  - To add a 5th project, copy one of the objects in the "projects" array below
    (including the outer { } and comma) and edit its fields. Order in this file
    is the order it appears on the page.
*/

window.SITE_DATA = {

  name: "David Nguyen",
  headlineHighlight: "the circuits",
  tagline: "Power electronics and embedded systems engineer — resonant converters, control loops, and the boards that make hardware products actually run. Currently building the hardware for AuraSleep and searching for my final co-op.",
  meta: ["Simon Fraser University", "Burnaby, BC, Canada", "Open to Co-op — [Spring - Summer/ 2027]"],

  resumeLink: "resume.pdf",
  email: "dtn7@sfu.ca",
  github: "https://github.com/thentd2003",
  linkedin: "https://linkedin.com/in/david-nguyen-53377a268",

  projects: [
    {
      designator: "U1",
      status: "Flagship · In Progress",
      statusClass: "is-flagship",
      title: "AuraSleep — Smart Sleep Mask",
      blurb: "Founder & hardware lead on a sleep-tech wearable — currently taking the hardware from concept to a first working milestone.",
      tags: ["PCB Design", "Embedded Systems", "Product Dev"],
      image: null,
      imageAlt: "AuraSleep sleep mask prototype",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/aurasleep",
      linkLabel: "Files",
      details: {
        "Problem": "Founded AuraSleep to build a wearable sleep mask that combines comfort with onboard sensing/intervention hardware — moving from an engineering background into leading full product development.",
        "My role": "Founder and hardware lead — driving the hardware roadmap from concept toward a first working prototype, alongside product and go-to-market decisions.",
        "Status": "[Add current milestone — e.g. \"first working prototype achieved MM/YYYY\" — plus any specs, sensor list, or power budget once finalized.]"
      }
    },
    {
      designator: "U2",
      status: "Complete",
      statusClass: "",
      title: "Half-Bridge Resonant Induction Heater",
      blurb: "An LCLR resonant induction heater built for a hardware startup, now mid-redesign toward closed-loop PLL frequency control.",
      tags: ["Half-Bridge", "LCLR Resonant Tank", "PLL Control"],
      image: null,
      imageAlt: "Half-bridge induction heater build",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/induction-heater",
      linkLabel: "Files",
      details: {
        "Problem": "Built for a hardware startup that needed a working induction heating stage — a half-bridge driving an LCLR resonant tank to deliver controlled heating power.",
        "Current work": "Redesigning the control approach toward closed-loop PLL tracking, so the drive frequency follows the tank's resonant point automatically instead of running open-loop.",
        "Outcome": "[Add power delivered, switching frequency range, and what the PLL redesign is expected to improve once complete.]"
      }
    },
    {
      designator: "U3",
      status: "In-Progress",
      statusClass: "",
      title: "Automated Motorized Soccer Tripod",
      blurb: "[Add a one-sentence description of what this device does and why you built it.]",
      tags: ["Motor Control", "Embedded Systems", "Automation"],
      image: null,
      imageAlt: "Automated motorized soccer tripod",
      mediaLabel: "+ Add photo",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/soccer-tripod",
      linkLabel: "Files",
      details: {
        "Problem": "[Describe what motivated this build — e.g. hands-free filming of soccer practice or games.]",
        "Approach": "[Describe the mechanical and control design — motors, tracking method, and key decisions you made.]",
        "Outcome": "[Add the result — tracking accuracy, reliability, or a link to demo footage.]"
      }
    },
    {
      designator: "U4",
      status: "Complete",
      statusClass: "",
      title: "High-Voltage Flyback Converter",
      blurb: "My first fully independent power electronics design, built during a co-op in analytic/scientific instrumentation, with full EMC/EMI test verification.",
      tags: ["Flyback Topology", "EMC/EMI Testing", "PCB Layout"],
      image: null,
      imageAlt: "HV flyback converter board",
      mediaLabel: "+ Add photo / schematic",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/flyback-converter",
      linkLabel: "Files",
      details: {
        "Problem": "[Describe what system needed high voltage and why a flyback topology was chosen over alternatives.]",
        "Approach": "Designed the converter from topology selection through transformer specification, feedback/control design, and PCB layout, then carried it through formal EMC/EMI compliance testing.",
        "Outcome": "[Add input/output voltage, power level, efficiency, and EMC/EMI test results — these numbers do more to convince a reviewer than any adjective.]"
      }
    },
    {
      designator: "U5",
      status: "Complete",
      statusClass: "",
      title: "RISC-V Pipeline Simulator",
      blurb: "A cycle-accurate RISC-V pipeline simulator in C, built with a 4-person team across four milestones for a computer architecture course.",
      tags: ["C", "Computer Architecture", "Team of 4"],
      image: null,
      imageAlt: "RISC-V pipeline simulator terminal output",
      mediaLabel: "+ Add screenshot",
      link: "https://github.com/thentd2003/thentd2003.github.io/tree/main/projects/riscv-simulator",
      linkLabel: "Files",
      details: {
        "Milestones": "Basic 5-stage pipeline → hazard detection & forwarding → cache integration → a final milestone of self-designed architectural extensions.",
        "My contribution": "[Specify which stages/modules you personally implemented — e.g. hazard unit, cache model, or the custom-innovation milestone.]",
        "Outcome": "[Add the result of the graded demo, and any performance/correctness metrics from your test suite.]"
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
