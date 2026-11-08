# JIMBONES — Digitone II inspired R&D proposals (2026-11-08)
These are virtual research assignments, not autonomous agents or implemented features. Do not copy proprietary Elektron firmware or assets. No USB audio in this candidate. Preserve six legacy voices, kits, working v1.8.02 and ARM headroom.

## JB-14 Transient Designer — P1
Research a per-voice attack/sustain transient shaper plus optional synthesised click/noise transient inspired by Digitone II FM Drum's transient/body/noise architecture (not a claim Digitone II has a conventional transient shaper). Prototype host-testable envelope detector and gain law; preserve low-level dynamics; bound latency, CPU and output peaks. Snare/toms first; velocity sensitivity. Reviewer independently measures four simultaneous hits and near-rail behavior.

## JB-15 FM Drum Body/Noise — P1
Investigate lightweight 3-operator FM drum with pitch sweep, body/noise envelope, transient blend and velocity routing; compare CPU with existing JIMBONES metallic FM, avoiding duplication. Reviewer validates legacy engine bypass and zero changes to protected kick.

## JB-16 Velocity Modulation Matrix — P1
Research bounded per-voice velocity routes to morph, transient, FM index, filter and tape drive, with explicit polarity/range, protection of morph A and optional kick exclusion. Reviewer tests low/high velocity and repeated hits.

## JB-17 Filter Routing & Comb Polarity — P2
Research pre/post-filter drive/SRR ordering and Comb+ / Comb− polarity options alongside the already requested Octatrack-style comb filter. Reviewer checks feedback stability, DC/rail safety, CPU and memory.

## JB-18 Bit/Sample-Rate Reduction — P2
Investigate per-voice bit depth and sample-rate reduction with smoothing and optional velocity mapping. Reviewer checks aliasing behavior, gain and 4-voice budget.

## JB-19 Detuned Oscillator Cluster — P3
Explore Swarmer-inspired detuned multi-oscillator timbres using CPU-bounded approximations, not literal seven-oscillator per voice requirement. Reviewer compares cost and sound value with existing wavetable engine.

## Shared gates
For each agent pair: architecture sketch, exact code touchpoints, host benchmark/prototype, reviewer counterexample tests, realistic ARM cost estimate, and recommendation to integrate/defer. Research is not permission to change working firmware. Existing JB-03 crunch fix and CPU telemetry remain higher priority than adding heavy DSP. Dub Step FX sends and blinking LED toggle remain separately tracked.
Sources: Elektron Digitone II official user manual OS 1.10 (https://elektron.se/wp-content/uploads/2025/03/Digitone-2-User-Manual_ENG_OS1.10_250320.pdf), and OS 1.12 manual sections (https://www.knobwerk.com/en/digitone2/manual/105_a2-syn-machines_a2-syn-machines.md).
